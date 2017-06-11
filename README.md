
# php-handlebars

[![Build Status](https://travis-ci.org/jbboehr/php-handlebars.svg?branch=master)](https://travis-ci.org/jbboehr/php-handlebars)
[![Coverage Status](https://coveralls.io/repos/jbboehr/php-handlebars/badge.svg?branch=master&service=github)](https://coveralls.io/github/jbboehr/php-handlebars?branch=master)
[![License](https://img.shields.io/badge/license-BSD-brightgreen.svg)](LICENSE.md)

PHP bindings for [handlebars.c](https://github.com/jbboehr/handlebars.c). See also [handlebars.php](https://github.com/jbboehr/handlebars.php).


## Installation


### PPA

```bash
sudo apt-add-repository ppa:jbboehr/handlebars
sudo apt-get update
sudo apt-get install php-handlebars
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


#### Ubuntu

```bash
sudo apt-get install php5-dev
git clone https://github.com/jbboehr/php-handlebars.git --recursive
cd php-handlebars
phpize
./configure
make
make test
sudo make install
echo extension=handlebars.so | sudo tee /etc/php5/mods-available/handlebars.ini
sudo php5enmod handlebars
```


## Usage

```php
$vm = new Handlebars\VM();

echo $vm->render('{{foo}}', array('foo' => 'bar'));

echo $vm->renderFile('/path/to/foo.hbs', array('foo' => 'bar'));
```

See the [examples](examples) folder for more examples.


## License

This project is licensed under the [Simplified BSD License](LICENSE.md).

