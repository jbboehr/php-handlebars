#!/usr/bin/env bash

set -e

#INSTALLED_HANDLEBARS_VERSION=`handlebarsc --version 2>&1 | awk '{ print $2 }'`
#if [ ! -f $HOME/build/include/handlebars.h ] || [ "$INSTALLED_HANDLEBARS_VERSION" != "v$LIBHANDLEBARS_VERSION" ]; then
    git clone -b $LIBHANDLEBARS_VERSION https://github.com/jbboehr/handlebars.c handlebars-c --recursive
    cd handlebars-c
    ./bootstrap
    ./configure --prefix=$HOME/build
    touch src/handlebars_scanners.c
    make install
    cd ..
#fi

git clone -b $PHP_PSR_VERSION https://github.com/jbboehr/php-psr.git
cd php-psr
phpize
./configure
make install
echo "extension=psr.so" >> ~/.phpenv/versions/$(phpenv version-name)/etc/php.ini
cd ..

