--TEST--
Handlebars\Opcode reflection (PHP 7)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID >= 80000 ) die('skip ');
if( !class_exists('Handlebars\\Opcode') ) die('skip handlebars AST not enabled');
?>
--FILE--
<?php
echo preg_replace('/\?([\w\\\\]+)/', '$1 or NULL', (new ReflectionClass(Handlebars\Opcode::CLASS)));
--EXPECT--
Class [ <internal:handlebars> class Handlebars\Opcode ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ <default> public $opcode ]
    Property [ <default> public $args ]
  }

  - Methods [1] {
    Method [ <internal:handlebars, ctor> public method __construct ] {

      - Parameters [2] {
        Parameter #0 [ <required> string $opcode ]
        Parameter #1 [ <required> array $args ]
      }
    }
  }
}
