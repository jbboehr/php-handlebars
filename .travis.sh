
set -e

if [ ! -f $HOME/build/include/check.h ]; then
    wget http://downloads.sourceforge.net/project/check/check/0.9.14/check-0.9.14.tar.gz
    tar xfv check-0.9.14.tar.gz
    cd check-0.9.14
    ./configure --prefix=$HOME/build
    make
    make install
    cd ..
    rm -Rf check-0.9.14.tar.gz check-0.9.14
fi

if [ ! -f $HOME/build/bin/bison ]; then
    wget http://gnu.mirror.iweb.com/bison/bison-3.0.2.tar.gz
    tar xfv bison-3.0.2.tar.gz
    cd bison-3.0.2
    ./configure --prefix=$HOME/build
    make
    make install
    cd ..
    rm -Rf bison-3.0.2.tar.gz bison-3.0.2
fi

if [ ! -f $HOME/build/include/handlebars.h ]; then
    wget https://github.com/jbboehr/handlebars.c/archive/v0.3.1.tar.gz
    tar xfv v0.3.1.tar.gz
    cd handlebars.c-0.3.1
    ./bootstrap
    ./configure --prefix=$HOME/build
    make install
    cd ..
    rm -Rf v0.3.1.tar.gz handlebars.c-0.3.1
fi

