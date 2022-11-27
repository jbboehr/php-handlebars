#!/usr/bin/env bash

set -o errexit -o pipefail

source .github/scripts/fold.sh

export PS4=' \e[33m$(date +"%H:%M:%S"): $BASH_SOURCE@$LINENO ${FUNCNAME[0]} -> \e[0m'

export DEFAULT_LIBHANDLEBARS_VERSION=`jq -r '.LIBHANDLEBARS_VERSION' .github/scripts/vars.json`
export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-$DEFAULT_LIBHANDLEBARS_VERSION}

export DEFAULT_PHP_PSR_VERSION=`jq -r '.PHP_PSR_VERSION' .github/scripts/vars.json`
export PHP_PSR_VERSION=${PHP_PSR_VERSION:-$DEFAULT_PHP_PSR_VERSION}

export AST=${AST:-false}
export PSR=${PSR:-false}
export COVERAGE=${COVERAGE:-true}
export HARDENING=${HARDENING:-true}

export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE=${TEST_PHP_EXECUTABLE:-`which php`}

export PREFIX="${HOME}/build"
export PATH="${PREFIX}/bin:${PATH}"
export CFLAGS="-L${PREFIX}/lib ${CFLAGS}"
export CPPFLAGS="-I${PREFIX}/include ${CPPFLAGS}"
export PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}"

mkdir -p ${PREFIX}/bin ${PREFIX}/lib/pkgconfig ${PREFIX}/include

if [[ "${HARDENING}" != "false" ]]; then
	export CFLAGS="$CFLAGS -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector-strong"
	export CFLAGS="$CFLAGS -specs=`pwd`/.github/scripts/redhat-hardened-cc1 -specs=`pwd`/.github/scripts/redhat-hardened-ld"
	export CFLAGS="$CFLAGS -fPIC -DPIC"
	# this is not supported on travis: -fcf-protection
    # this is only supported on gcc >= ~8.3 (?): -fstack-clash-protection
fi

function install_libhandlebars() (
    set -o errexit -o pipefail -o xtrace

    local dir=third-party/handlebars-c
    rm -rf ${dir}
    git clone https://github.com/jbboehr/handlebars.c.git ${dir} --recursive
    cd ${dir}
    git checkout ${LIBHANDLEBARS_VERSION}
    git submodule update --init --recursive
    ./bootstrap
    trap "cat config.log" ERR
    ./configure --prefix=${PREFIX} --enable-compile-warnings=yes --disable-Werror CFLAGS="${CFLAGS}"
    trap - ERR
    touch src/handlebars_scanners.c
    make all install
)

function install_php_psr() (
    set -o errexit -o pipefail -o xtrace

    local dir=third-party/php-psr
    rm -rf ${dir}
    git clone https://github.com/jbboehr/php-psr.git ${dir}
    cd ${dir}
    git checkout ${PHP_PSR_VERSION}
    phpize
    trap "cat config.log" ERR
    ./configure --prefix=${PREFIX} CFLAGS="${CFLAGS}"
    trap - ERR
    make
)

function install_php_handlebars() (
    set -o errexit -o pipefail -o xtrace

    if [[ "${COVERAGE}" = "true" ]]; then
        export CFLAGS="-fprofile-arcs -ftest-coverage ${CFLAGS}"
        export LDFLAGS="--coverage ${LDFLAGS}"
    fi

    local extra_configure_flags=""

    if [[ "${AST}" == "true" ]]; then
        extra_configure_flags="${extra_configure_flags} --enable-handlebars-ast"
    else
        extra_configure_flags="${extra_configure_flags} --disable-handlebars-ast"
    fi

    if [[ "${HARDENING}" != "false" ]]; then
        extra_configure_flags="${extra_configure_flags} --enable-handlebars-hardening"
    else
        extra_configure_flags="${extra_configure_flags} --disable-handlebars-hardening"
    fi

    phpize
    trap "cat config.log" ERR
    ./configure --enable-handlebars \
        --enable-compile-warnings=error \
        ${extra_configure_flags} \
        CFLAGS="${CFLAGS}"
    trap - ERR
    make
)

function update_submodules() (
    set -o errexit -o pipefail -o xtrace

    git submodule update --init --recursive
)

function before_install() (
    set -o errexit -o pipefail

    cifold "update submodules" update_submodules
)

function install() (
    set -o errexit -o pipefail

    cifold "install handlebars.c from source" install_libhandlebars
    if [[ ! -z "${PHP_PSR_VERSION}" ]]; then
        cifold "install php-psr from source" install_php_psr
    fi
    cifold "main build step" install_php_handlebars
)

function generate_handlebars_tests() (
    set -o errexit -o pipefail -o xtrace

    php generate-tests.php
)

function initialize_coverage() (
    set -o errexit -o pipefail -o xtrace

    lcov --directory . --zerocounters
    lcov --directory . --capture --compat-libtool --initial --output-file coverage.info
)

function before_script() (
    set -o errexit -o pipefail

    cifold "generate tests from spec" generate_handlebars_tests
    if [[ "${COVERAGE}" = "true" ]]; then
        cifold "initialize coverage" initialize_coverage
    fi
)

function run_examples() (
    set -o errexit -o pipefail -o xtrace

    for i in `find examples -name "*.php" -not -name benchmarks.php`; do
        printf "\nExecuting example ${i}:\n"
        php -d extension=modules/handlebars.so $i
    done

    return 0
)

function run_stubs() {
    set -o errexit -o pipefail -o xtrace

    php handlebars.stub.php
    php handlebars-ast.stub.php

    return 0
}

function test_php_handlebars() (
    set -o errexit -o pipefail -o xtrace

    # we can save a fair bit of time by removing these tests if AST is not enabled
    if [[ "${AST}" != "true" ]]; then
        rm -rf tests/handlebars/export tests/handlebars/spec/parser tests/handlebars/spec/tokenizer
    fi

    local extra_flags=""
    if [ "$PSR" = "true" ]; then
        extra_flags="-d extension=third-party/php-psr/modules/psr.so"
    fi

    php run-tests.php -n \
        ${extra_flags} \
        -d extension=modules/handlebars.so
)

function script() (
    set -o errexit -o pipefail -o xtrace

    cifold "main test suite" test_php_handlebars
    cifold "stubs" run_stubs
    cifold "examples" run_examples
    cifold "benchmarks" ./examples/benchmarks.sh
)

function upload_coverage() (
    set -o errexit -o pipefail -o xtrace

    lcov --no-checksum --directory . --capture --compat-libtool --output-file coverage.info
    lcov --remove coverage.info "/usr*" \
        --remove coverage.info "*/.phpenv/*" \
        --remove coverage.info "*/xxh*.h" \
        --remove coverage.info "/home/travis/build/include/*" \
        --compat-libtool \
        --output-file coverage.info
)

function after_success() (
    set -o errexit -o pipefail

    if [[ "${COVERAGE}" = "true" ]]; then
        cifold "upload coverage" upload_coverage
    fi
)

function after_failure() (
    # set -o errexit -o pipefail

    for i in `find tests -name "*.out" 2>/dev/null`; do
        echo "-- START ${i}";
        cat ${i};
        echo "-- END";
    done
    for i in `find tests -name "*.mem" 2>/dev/null`; do
        echo "-- START ${i}";
        cat ${i};
        echo "-- END";
    done
)

function run_all() (
    set -o errexit -o pipefail
    trap after_failure ERR
    before_install
    install
    before_script
    script
    after_success
)

if [ "$1" == "run-all-now" ]; then
    run_all
fi
