FROM fedora:31
RUN dnf groupinstall 'Development Tools' -y
RUN dnf install php-devel -y
RUN dnf install gcc automake autoconf libtool libyaml-devel json-c-devel libtalloc-devel pcre-devel lmdb-devel check-devel -y

# build handlebars
ARG LIBHANDLEBARS_VERSION=master
RUN git clone https://github.com/jbboehr/handlebars.c && \
    cd handlebars.c && \
    git checkout $LIBHANDLEBARS_VERSION && \
    git submodule update --init && \
    ./bootstrap && \
    ./configure && \
    make && \
    make test && \
    make install && \
    cd .. && \
    rm -rf handlebars.c

# build extension
ADD . .
RUN phpize && \
    ./configure PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig" CFLAGS="$CFLAGS -Wno-shadow -Wno-error=shadow" && \
    make

CMD ["./.ci/entry.sh"]

