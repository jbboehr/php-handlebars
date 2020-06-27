
ARG BASE_IMAGE=fedora:latest

# image0
FROM ${BASE_IMAGE}
ARG PHP_VERSION
ARG HANDLEBARSC_VERSION=master
WORKDIR /build

# handlebars.c
RUN dnf groupinstall 'Development Tools' -y
RUN dnf install \
    git-all \
    gcc \
    automake \
    autoconf \
    libtool \
    libyaml-devel \
    json-c-devel \
    libtalloc-devel \
    pcre-devel \
    lmdb-devel \
    check-devel \
    php-devel \
    -y
RUN git clone https://github.com/jbboehr/handlebars.c.git
WORKDIR /build/handlebars.c
RUN git checkout $HANDLEBARSC_VERSION
RUN autoreconf -fiv
RUN ./configure \
        --prefix=/usr/local \
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
RUN sudo make install
RUN sudo ldconfig

# php-handlebars
WORKDIR /build/php-handlebars
ADD . .
RUN phpize
RUN ./configure \
        --disable-handlebars-ast \
        CFLAGS="-O3" \
        PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
RUN make
RUN make install

# image1
FROM ${BASE_IMAGE}
RUN dnf install php-cli libyaml json-c libtalloc lmdb -y
# this probably won't work on other arches
COPY --from=0 /usr/lib64/php/modules/handlebars.so /usr/lib64/php/modules/handlebars.so
# please forgive me
COPY --from=0 /usr/lib64/php/build/run-tests.php /usr/local/lib/php/build/run-tests.php
RUN echo extension=handlebars.so | sudo tee /etc/php.d/90-handlebars.ini
