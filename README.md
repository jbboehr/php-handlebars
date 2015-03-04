
# php-handlebars

[![Build Status](https://travis-ci.org/jbboehr/php-handlebars.svg?branch=master)](https://travis-ci.org/jbboehr/php-handlebars)

PHP bindings for the handlebars.c lexer, parser, and compiler. Use with [handlebars.c](https://github.com/jbboehr/handlebars.c) and [handlebars.php](https://github.com/jbboehr/handlebars.php).


## Installation


### PPA

```bash
sudo apt-add-repository ppa:jbboehr/handlebars
sudo apt-get update
sudo apt-get install handlebarsc libhandlebars-dev
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

# precise
echo extension=handlebars.so | sudo tee /etc/php5/conf.d/handlebars.ini

# trusty
echo extension=handlebars.so | sudo tee /etc/php5/mods-available/handlebars.ini
sudo php5enmod handlebars
```


## License

This project is licensed under the [LGPLv3](http://www.gnu.org/licenses/lgpl-3.0.txt).
handlebars.js is licensed under the [MIT license](http://opensource.org/licenses/MIT).
