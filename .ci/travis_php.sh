#!/usr/bin/env bash

export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-master}
export PHP_PSR_VERSION=${PHP_PSR_VERSION:-master}
export COVERAGE=${COVERAGE:-true}

export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE=`which php`

export PREFIX="${HOME}/build"
export PATH="${PREFIX}/bin:${PATH}"
export CFLAGS="-L${PREFIX}/lib ${CFLAGS}"
export CPPFLAGS="-I${PREFIX}/include ${CPPFLAGS}"
export PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}"

function install_libhandlebars() (
    set -e -o pipefail

    local dir=third-party/handlebars-c
    rm -rf ${dir}
    git clone -b ${LIBHANDLEBARS_VERSION} https://github.com/jbboehr/handlebars.c.git ${dir} --recursive
    cd ${dir}
    ./bootstrap
    ./configure --prefix=${PREFIX}
    touch src/handlebars_scanners.c
    make all install
)

function install_php_psr() (
    set -e -o pipefail

    local dir=third-party/php-psr
    rm -rf ${dir}
    git clone -b ${PHP_PSR_VERSION} https://github.com/jbboehr/php-psr.git ${dir}
    cd ${dir}
    phpize
    ./configure --prefix=${PREFIX}
    make
)

function before_install() (
    set -e -o pipefail

    git submodule update --init --recursive

    # Don't install this unless we're actually on travis
    if [[ "${COVERAGE}" = "true" ]] && [[ "${TRAVIS}" = "true" ]]; then
        gem install coveralls-lcov
    fi
)

function install() (
    set -e -o pipefail

    install_libhandlebars
    install_php_psr

    phpize
    if [[ "${COVERAGE}" = "true" ]]; then
        ./configure --enable-handlebars \
            CFLAGS="-fprofile-arcs -ftest-coverage ${CFLAGS}" \
            LDFLAGS="--coverage ${LDFLAGS}"
    else
        ./configure --enable-handlebars
    fi
    make
)

function before_script() (
    set -e -o pipefail

    echo "Generating unit tests from spec"
    php generate-tests.php

    if [[ "${COVERAGE}" = "true" ]]; then
        echo "Initializing coverage"
        lcov --directory . --zerocounters
        lcov --directory . --capture --compat-libtool --initial --output-file coverage.info
    fi
)

function script() (
    set -e -o pipefail

    echo "Running main test suite"
    php run-tests.php -n \
        -d extension=third-party/php-psr/modules/psr.so \
        -d extension=modules/handlebars.so
)

function after_success() (
    set -e -o pipefail

    if [[ "${COVERAGE}" = "true" ]]; then
        echo "Processing coverage"
        lcov --no-checksum --directory . --capture --compat-libtool --output-file coverage.info
        lcov --remove coverage.info "/usr*" \
            --remove coverage.info "*/.phpenv/*" \
            --remove coverage.info "*/xxh*.h" \
            --remove coverage.info "/home/travis/build/include/*" \
            --compat-libtool \
            --output-file coverage.info

        echo "Uploading coverage"
        coveralls-lcov coverage.info
    fi
)

function after_failure() (
    set -e -o pipefail

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
    set -e
    trap after_failure ERR
    before_install
    install
    before_script
    script
    after_success
)
