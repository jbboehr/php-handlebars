#!/usr/bin/env bash

set -ex -o pipefail

# config
export DEFAULT_LIBHANDLEBARS_VERSION=`jq -r '.LIBHANDLEBARS_VERSION' .ci/vars.json`
export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-$DEFAULT_LIBHANDLEBARS_VERSION}

export DEFAULT_PHP_PSR_VERSION=`jq -r '.PHP_PSR_VERSION' .ci/vars.json`
export PHP_PSR_VERSION=${PHP_PSR_VERSION:-$DEFAULT_PHP_PSR_VERSION}

export PHP_VERSION=${PHP_VERSION:-"7.4"}

export COVERAGE=${COVERAGE:-false}
export CFLAGS="-Wno-shadow -Wno-error=shadow"

# install deps
export DEBIAN_FRONTEND=noninteractive
sudo add-apt-repository ppa:ondrej/php
sudo apt-get update
sudo apt-get install -y libjson-c-dev liblmdb-dev libpcre3-dev libtalloc-dev libyaml-dev php-dev php${PHP_VERSION}-dev

# source and execute script used in travis
source .ci/travis_php.sh
run_all
