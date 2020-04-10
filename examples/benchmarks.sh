#!/usr/bin/env bash

function run() {
    php -d extension=modules/handlebars.so \
        -d handlebars.cache.enable=1 \
        -d handlebars.cache.enable_cli=1 \
        $@ \
        examples/benchmarks.php
}

run -d handlebars.cache.enable=0
run -d handlebars.cache.backend=simple
run -d handlebars.cache.backend=lmdb
run -d handlebars.cache.backend=mmap




