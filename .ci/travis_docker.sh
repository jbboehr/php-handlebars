#!/usr/bin/env bash

export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-master}
export DOCKER_TAG=php-handlebars:${DOCKER_IMAGE}

function before_install() (
    return 0
)

function install() {
    set -e -o pipefail

    docker build \
        --build-arg LIBHANDLEBARS_VERSION="${LIBHANDLEBARS_VERSION}" \
        -f .ci/${DOCKER_IMAGE}.Dockerfile \
        -t ${DOCKER_TAG} \
        .
    return 0
}

function before_script() {
    return 0
}

function script() (
    set -e -o pipefail

    docker run ${DOCKER_TAG}
)

function after_success() (
    return 0
)

function after_failure() {
    return 0
}

function run_all() (
    set -e
    trap after_failure ERR
    before_install
    install
    before_script
    script
    after_success
)
