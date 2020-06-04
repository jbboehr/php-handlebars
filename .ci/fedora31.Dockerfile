FROM fedora:31
RUN dnf groupinstall 'Development Tools' -y
RUN dnf install php-devel -y
RUN dnf install gcc automake autoconf libtool libyaml-devel json-c-devel libtalloc-devel pcre-devel lmdb-devel check-devel -y

# build handlebars
ARG LIBHANDLEBARS_VERSION=master
WORKDIR /build/handlebars.c
RUN git clone https://github.com/jbboehr/handlebars.c . && \
    git checkout $LIBHANDLEBARS_VERSION && \
    git submodule update --init && \
    ./bootstrap && \
    ./configure --prefix=/usr/local && \
    make && \
    make test && \
    make install && \
    ldconfig && \
    cd .. && \
    rm -rf handlebars.c

# build extension
WORKDIR /build/php-handlebars
ADD . .
RUN phpize && \
    ./configure --prefix=/usr/local PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig" CFLAGS="$CFLAGS -Wno-shadow -Wno-error=shadow -g -O2" && \
    make clean && \
    make

CMD "./.ci/entry.sh"
