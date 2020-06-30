#!/usr/bin/env bash

set -o errexit -o pipefail

source .ci/fold.sh

export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-master}
export DOCKER_TAG=php-handlebars:${DOCKER_IMAGE}

function before_install() (
    return 0
)

function docker_build() (
    set -o errexit -o pipefail -o xtrace

    docker build \
        --build-arg LIBHANDLEBARS_VERSION="${LIBHANDLEBARS_VERSION}" \
        -f .ci/${DOCKER_IMAGE}.Dockerfile \
        -t ${DOCKER_TAG} \
        .
)

function install() {
    set -o errexit -o pipefail

    cifold "docker build" docker_build
}

function before_script() {
    return 0
}

function docker_run() (
    set -o errexit -o pipefail -o xtrace

    docker run ${DOCKER_TAG}
)

function script() (
    set -e -o pipefail

    cifold "docker build" docker_build
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

if [ "$1" == "run-all-now" ]; then
    run_all
fi
