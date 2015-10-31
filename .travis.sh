
set -e

if [ ! -f $HOME/build/include/check.h ]; then
    wget http://downloads.sourceforge.net/project/check/check/0.9.14/check-0.9.14.tar.gz
    tar xfv check-0.9.14.tar.gz
    cd check-0.9.14
    ./configure --prefix=$HOME/build
    make
    make install
    cd ..
    rm check-0.9.14.tar.gz
fi

if [ ! -f $HOME/build/bin/bison ]; then
    wget http://gnu.mirror.iweb.com/bison/bison-3.0.2.tar.gz
    tar xfv bison-3.0.2.tar.gz
    cd bison-3.0.2
    ./configure --prefix=$HOME/build
    make
    make install
    cd ..
    rm bison-3.0.2.tar.gz
fi

INSTALLED_HANDLEBARS_VERSION=`handlebarsc --version 2>&1 | awk '{ print $2 }'`
if [ ! -f $HOME/build/include/handlebars.h ] || [ "$INSTALLED_HANDLEBARS_VERSION" != "v$LIBHANDLEBARS_VERSION" ]; then
    git clone -b v$LIBHANDLEBARS_VERSION https://github.com/jbboehr/handlebars.c handlebars-c --recursive
    cd handlebars-c
    ./bootstrap
    ./configure --prefix=$HOME/build
    make install
    cd ..
fi

