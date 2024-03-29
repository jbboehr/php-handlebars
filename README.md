
# php-handlebars

[![GitHub Build Status](https://github.com/jbboehr/php-handlebars/workflows/ci/badge.svg)](https://github.com/jbboehr/php-handlebars/actions?query=workflow%3Aci)
[![Coverage Status](https://coveralls.io/repos/jbboehr/php-handlebars/badge.svg?branch=master&service=github)](https://coveralls.io/github/jbboehr/php-handlebars?branch=master)
[![License](https://img.shields.io/badge/license-BSD-brightgreen.svg)](LICENSE.md)

PHP bindings for [handlebars.c](https://github.com/jbboehr/handlebars.c).


## Installation


### Nix / NixOS

```bash
nix-env -i -f https://github.com/jbboehr/php-handlebars/archive/v0.9.1.tar.gz
```

with a custom version of PHP:

```bash
nix-env -i -f https://github.com/jbboehr/php-handlebars/archive/v0.9.1.tar.gz \
    --arg php '(import <nixpkgs> {}).php73'
```

or, in a `.nix` file:

```nix
(import <nixpkgs> {}).callPackage (import (fetchTarball {
  url = https://github.com/jbboehr/php-handlebars/archive/v0.9.0.tar.gz;
  sha256 = "1dgq05b11hg6llfjl8zwxzvixf2g7vn4np4ihcl589jmxdal9np4";
})) {}
```


### RPM repository

The extension is available in [Remi's repository](https://rpms.remirepo.net/):

**Fedora** (change 24 to match your Fedora version)

```bash
dnf install https://rpms.remirepo.net/fedora/remi-release-24.rpm
dnf install --enablerepo=remi php-pecl-handlebars
```

**RHEL/CentOS** (for default PHP in base repository)

```bash
yum install https://rpms.remirepo.net/enterprise/remi-release-7.rpm
yum install php-pecl-handlebars
```


### Source

Install [handlebars.c](https://github.com/jbboehr/handlebars.c)


#### Linux / OSX

Prerequisite packages are:

- PHP development headers and tools
- `gcc` >= 4.4 | `clang` >= 3.x | `vc` >= 11
- GNU `make` >= 3.81
- `automake`
- `autoconf`
- [`handlebars.c`](https://github.com/jbboehr/handlebars.c) and all of its dependencies (`talloc`, `json-c`, `libyaml`)
- (optional) [`php-psr`](https://github.com/jbboehr/php-psr)

You will need the PHP development headers. If PHP was manually installed, these should be available by default. Otherwise, you will need to fetch them from a repository.

```bash
git clone https://github.com/jbboehr/php-handlebars.git
cd php-handlebars
phpize
./configure
make
make test
sudo make install
```

If you have specific PHP versions running:

```bash
git clone https://github.com/jbboehr/php-handlebars.git
cd php-handlebars
/usr/local/bin/phpize
./configure --with-php-config=/usr/local/bin/php-config
make
make test
sudo make install
```

Add the extension to your *php.ini*:

```ini
echo extension=handlebars.so | tee -a /path/to/your/php.ini
```

Finally, _restart the web server_.


## Usage

```php
$vm = new Handlebars\VM();

echo $vm->render('{{foo}}', array('foo' => 'bar'));

echo $vm->renderFile('/path/to/foo.hbs', array('foo' => 'bar'));
```

See the [examples](examples) folder for more examples.


## License

This project is licensed under the [Simplified BSD License](LICENSE.md) (BSD-2-Clause).

