--TEST--
Handlebars\SafeString reflection
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 70400 ) die('skip not applicable to < PHP 7.4');
?>
--FILE--
<?php
echo preg_replace('/\?(\w+)/', '$1 or NULL', (new ReflectionClass(Handlebars\SafeString::CLASS)));
--EXPECT--
Class [ <internal:handlebars> class Handlebars\SafeString ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    Property [ <default> protected $value ]
  }

  - Methods [2] {
    Method [ <internal:handlebars, ctor> public method __construct ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $value ]
      }
    }

    Method [ <internal:handlebars> public method __toString ] {

      - Parameters [0] {
      }
      - Return [ string ]
    }
  }
}
