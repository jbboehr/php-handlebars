#!/usr/bin/env bash

set -ex -o pipefail

# config
export DEFAULT_LIBHANDLEBARS_VERSION=`jq -r '.LIBHANDLEBARS_VERSION' .github/scripts/vars.json`
export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-$DEFAULT_LIBHANDLEBARS_VERSION}

export DEFAULT_PHP_PSR_VERSION=`jq -r '.PHP_PSR_VERSION' .github/scripts/vars.json`
export PHP_PSR_VERSION=${PHP_PSR_VERSION:-$DEFAULT_PHP_PSR_VERSION}

export PHP_VERSION=${PHP_VERSION:-"7.4"}

# install handlebars.c
(
    brew install autoconf automake bats-core json-c libtool libyaml lmdb pkg-config talloc check lcov pcre
    git clone https://github.com/jbboehr/handlebars.c.git
    cd handlebars.c
    git checkout $LIBHANDLEBARS_VERSION
    autoreconf -fiv
    ./configure --disable-bison --disable-flex --disable-gperf --enable-compile-warnings=no --disable-testing-exports
    make
    sudo make install
)

# build and install and run tests
brew install php@$PHP_VERSION
export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE=${TEST_PHP_EXECUTABLE:-`which php`}
phpize
# I don't like this but can't figure out how to get rid of:
# ld: warning: dylib (/usr/local/lib/libtalloc.dylib) was built for newer macOS version (12.4) than being linked (12.0)
./configure --enable-compile-warnings=no
make
make test
