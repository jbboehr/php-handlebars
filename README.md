
# php-handlebars

[![Build Status](https://travis-ci.org/jbboehr/php-handlebars.svg?branch=master)](https://travis-ci.org/jbboehr/php-handlebars)
[![Build Status](https://ci.appveyor.com/api/projects/status/r2jjw6bvx16vsnoq?svg=true)](https://ci.appveyor.com/project/jbboehr/php-handlebars)
[![Coverage Status](https://coveralls.io/repos/jbboehr/php-handlebars/badge.svg?branch=master&service=github)](https://coveralls.io/github/jbboehr/php-handlebars?branch=master)
[![License](https://img.shields.io/badge/license-BSD-brightgreen.svg)](LICENSE.md)

PHP bindings for [handlebars.c](https://github.com/jbboehr/handlebars.c). See also [handlebars.php](https://github.com/jbboehr/handlebars.php).


## Installation


### Nix / NixOS

```bash
nix-env -i -f https://github.com/jbboehr/php-handlebars/archive/master.tar.gz
```

with a custom version of PHP:

```bash
nix-env -i -f https://github.com/jbboehr/php-handlebars/archive/master.tar.gz --arg php '(import <nixpkgs> {}).php71'
```

or, in a `.nix` file:

```nix
(import <nixpkgs> {}).callPackage (import (fetchTarball {
  url = https://github.com/jbboehr/php-handlebars/archive/v0.8.3.tar.gz;
  sha256 = "17q3nv3b048bf873g12jzgnj11g4vzwww2rwcwfh7l46f34k3x8d";
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

