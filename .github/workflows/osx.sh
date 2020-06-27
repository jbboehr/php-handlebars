#!/usr/bin/env bash

set -ex -o pipefail

# config
export DEFAULT_LIBHANDLEBARS_VERSION=`jq -r '.LIBHANDLEBARS_VERSION' .ci/vars.json`
export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-$DEFAULT_LIBHANDLEBARS_VERSION}

export DEFAULT_PHP_PSR_VERSION=`jq -r '.PHP_PSR_VERSION' .ci/vars.json`
export PHP_PSR_VERSION=${PHP_PSR_VERSION:-$DEFAULT_PHP_PSR_VERSION}

export PHP_VERSION=${PHP_VERSION:-"7.4"}

# install handlebars.c
(
    brew install autoconf automake bats json-c libtool libyaml lmdb pkg-config talloc check lcov pcre
    git clone https://github.com/jbboehr/handlebars.c.git
    cd handlebars.c
    git checkout $LIBHANDLEBARS_VERSION
    autoreconf -fiv
    ./configure --disable-bison --disable-flex --disable-gperf --enable-compile-warnings=yes --disable-testing-exports
    make
    sudo make install
)

# build and install and run tests
brew install php@$PHP_VERSION
export NO_INTERACTION=1
export REPORT_EXIT_STATUS=1
export TEST_PHP_EXECUTABLE=${TEST_PHP_EXECUTABLE:-`which php`}
phpize
./configure CFLAGS="-Wno-double-promotion -Wno-error=double-promotion -Wno-unknown-warning-option -Wno-error=unknown-warning-option -Wno-cast-align -Wno-error=cast-align -Wno-missing-braces -Wno-error=missing-braces"
make
make test
