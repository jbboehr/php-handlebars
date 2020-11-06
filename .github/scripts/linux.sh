#!/usr/bin/env bash

set -ex -o pipefail

# config
export DEFAULT_LIBHANDLEBARS_VERSION=`jq -r '.LIBHANDLEBARS_VERSION' .github/scripts/vars.json`
export LIBHANDLEBARS_VERSION=${LIBHANDLEBARS_VERSION:-$DEFAULT_LIBHANDLEBARS_VERSION}

export DEFAULT_PHP_PSR_VERSION=`jq -r '.PHP_PSR_VERSION' .github/scripts/vars.json`
export PHP_PSR_VERSION=${PHP_PSR_VERSION:-$DEFAULT_PHP_PSR_VERSION}

export PHP_VERSION=${PHP_VERSION:-"7.4"}

export COVERAGE=${COVERAGE:-false}
export CFLAGS="-Wno-shadow -Wno-error=shadow"

export SUDO=sudo

# install deps
export DEBIAN_FRONTEND=noninteractive
${SUDO} add-apt-repository ppa:ondrej/php
${SUDO} apt-get update
${SUDO} apt-get install -y build-essential libjson-c-dev liblmdb-dev libpcre3-dev libtalloc-dev libyaml-dev php-dev php${PHP_VERSION}-dev lcov
${SUDO} update-alternatives --set php /usr/bin/php${PHP_VERSION}
${SUDO} update-alternatives --set php-config /usr/bin/php-config${PHP_VERSION}
${SUDO} update-alternatives --set phpize /usr/bin/phpize${PHP_VERSION}
${SUDO} update-alternatives --config php

# source and execute script used in travis
source .github/scripts/suite.sh
run_all
