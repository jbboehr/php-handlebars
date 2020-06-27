ARG PHP_VERSION=7.4
ARG PHP_TYPE=alpine
ARG BASE_IMAGE=php:${PHP_VERSION}-${PHP_TYPE}

# image0
FROM ${BASE_IMAGE}
ARG PHP_VERSION
ARG HANDLEBARSC_VERSION=master
WORKDIR /build
RUN apk update && \
    apk --no-cache add alpine-sdk automake autoconf libtool talloc-dev json-c-dev yaml-dev \
        pcre-dev check-dev bats
RUN git clone https://github.com/jbboehr/handlebars.c.git
WORKDIR /build/handlebars.c
RUN git checkout $HANDLEBARSC_VERSION
RUN autoreconf -fiv
RUN ./configure \
        --prefix /usr/local/ \
        --enable-lto \
        --enable-static \
        --enable-hardening \
        --disable-shared \
        --disable-debug \
        --disable-code-coverage \
        --disable-lmdb \
        --enable-pthread \
        --disable-valgrind \
        --disable-testing-exports \
        --enable-compile-warnings=yes \
        --disable-Werror \
        RANLIB=gcc-ranlib \
        AR=gcc-ar \
        NM=gcc-nm \
        LD=gcc \
        CFLAGS="-O3"
RUN make
RUN make install

# php-handlebars
WORKDIR /build/php-handlebars
ADD . .
RUN phpize
RUN ./configure --disable-handlebars-ast \
        CFLAGS="-O3"
RUN make
RUN make install

# image1
FROM ${BASE_IMAGE}
RUN apk --no-cache add talloc json-c yaml
COPY --from=0 /usr/local/lib/php/extensions /usr/local/lib/php/extensions
RUN docker-php-ext-enable handlebars
ENTRYPOINT ["docker-php-entrypoint"]
