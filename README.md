
# php-handlebars

[![Build Status](https://travis-ci.org/jbboehr/php-handlebars.svg?branch=master)](https://travis-ci.org/jbboehr/php-handlebars)

PHP bindings for the handlebars.c lexer, parser, and compiler.


## Requirements

[handlebars.c](https://github.com/jbboehr/handlebars.c)


## Installation


### Ubuntu:

```bash
sudo apt-get install php5-dev
git clone https://github.com/jbboehr/php-handlebars.git --recursive
cd php-handlebars
phpize
./configure
make
make test
sudo make install
echo extension=handlebars.so | sudo tee /etc/php5/conf.d/handlebars.ini
```


## License

This project is licensed under the [LGPLv3](http://www.gnu.org/licenses/lgpl-3.0.txt).
handlebars.js is licensed under the [MIT license](http://opensource.org/licenses/MIT).
