#!/usr/bin/env bash

if [[ "${TRAVIS_PHP_VERSION}" != "" ]]; then
    export TRAVIS_LANGUAGE=php
    source .ci/travis_php.sh
elif [[ "${DOCKER_IMAGE}" != "" ]]; then
    source .ci/travis_docker.sh
else
    echo "Failed to detect build language"
    exit 1
fi
