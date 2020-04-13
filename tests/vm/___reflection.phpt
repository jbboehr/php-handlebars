--TEST--
Handlebars\VM reflection (PHP 8, with php-psr)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( !extension_loaded('psr') ) die('skip ');
if( PHP_VERSION_ID < 80000 ) die('skip ');
?>
--FILE--
<?php
echo preg_replace('/\?([\w\\\\]+)/', '$1 or NULL', (new ReflectionClass(Handlebars\VM::CLASS)));
--EXPECT--
Class [ <internal:handlebars> class Handlebars\VM extends Handlebars\BaseImpl implements Psr\Log\LoggerAwareInterface, Handlebars\Impl ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [4] {
    Property [ protected $logger ]
    Property [ protected Handlebars\Registry or NULL $decorators ]
    Property [ protected Handlebars\Registry or NULL $helpers ]
    Property [ protected Handlebars\Registry or NULL $partials ]
  }

  - Methods [13] {
    Method [ <internal:handlebars, ctor> public method __construct ] {

      - Parameters [1] {
        Parameter #0 [ <required> array or NULL $options ]
      }
    }

    Method [ <internal:handlebars, overwrites Handlebars\BaseImpl, prototype Handlebars\Impl> public method setHelpers ] {

      - Parameters [1] {
        Parameter #0 [ <required> Handlebars\Registry $helpers ]
      }
      - Return [ Handlebars\Impl ]
    }

    Method [ <internal:handlebars, overwrites Handlebars\BaseImpl, prototype Handlebars\Impl> public method setPartials ] {

      - Parameters [1] {
        Parameter #0 [ <required> Handlebars\Registry $partials ]
      }
      - Return [ Handlebars\Impl ]
    }

    Method [ <internal:handlebars> public method compile ] {

      - Parameters [2] {
        Parameter #0 [ <required> string $tmpl ]
        Parameter #1 [ <optional> array or NULL $options = <default> ]
      }
      - Return [ string ]
    }

    Method [ <internal:handlebars, overwrites Handlebars\Impl, prototype Handlebars\Impl> public method render ] {

      - Parameters [3] {
        Parameter #0 [ <required> string $tmpl ]
        Parameter #1 [ <optional> $context = <default> ]
        Parameter #2 [ <optional> array or NULL $options = <default> ]
      }
      - Return [ string ]
    }

    Method [ <internal:handlebars, overwrites Handlebars\Impl, prototype Handlebars\Impl> public method renderFile ] {

      - Parameters [3] {
        Parameter #0 [ <required> string $filename ]
        Parameter #1 [ <optional> $context = <default> ]
        Parameter #2 [ <optional> array or NULL $options = <default> ]
      }
      - Return [ string ]
    }

    Method [ <internal:handlebars> public method renderFromBinaryString ] {

      - Parameters [3] {
        Parameter #0 [ <required> string $binaryString ]
        Parameter #1 [ <optional> $context = <default> ]
        Parameter #2 [ <optional> array or NULL $options = <default> ]
      }
      - Return [ string ]
    }

    Method [ <internal:handlebars, inherits Handlebars\BaseImpl, prototype Handlebars\Impl> public method getHelpers ] {

      - Parameters [0] {
      }
      - Return [ Handlebars\Registry or NULL ]
    }

    Method [ <internal:handlebars, inherits Handlebars\BaseImpl, prototype Handlebars\Impl> public method getPartials ] {

      - Parameters [0] {
      }
      - Return [ Handlebars\Registry or NULL ]
    }

    Method [ <internal:handlebars, inherits Handlebars\BaseImpl, prototype Handlebars\Impl> public method getDecorators ] {

      - Parameters [0] {
      }
      - Return [ Handlebars\Registry or NULL ]
    }

    Method [ <internal:handlebars, inherits Handlebars\BaseImpl, prototype Handlebars\Impl> public method getLogger ] {

      - Parameters [0] {
      }
      - Return [ Psr\Logger\LoggerInterface or NULL ]
    }

    Method [ <internal:handlebars, inherits Handlebars\BaseImpl, prototype Handlebars\Impl> public method setDecorators ] {

      - Parameters [1] {
        Parameter #0 [ <required> Handlebars\Registry $decorators ]
      }
      - Return [ Handlebars\Impl ]
    }

    Method [ <internal:handlebars, inherits Handlebars\BaseImpl, prototype Handlebars\Impl> public method setLogger ] {

      - Parameters [1] {
        Parameter #0 [ <required> Psr\Log\LoggerInterface $logger ]
      }
      - Return [ Handlebars\Impl ]
    }
  }
}
