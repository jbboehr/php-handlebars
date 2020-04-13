--TEST--
Handlebars\BaseImpl reflection (PHP 7, without php-psr)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( extension_loaded('psr') ) die('skip ');
if( PHP_VERSION_ID >= 80000 ) die('skip ');
?>
--FILE--
<?php
echo preg_replace('/\?([\w\\\\]+)/', '$1 or NULL', (new ReflectionClass(Handlebars\BaseImpl::CLASS)));
--EXPECT--
Class [ <internal:handlebars> abstract class Handlebars\BaseImpl implements Handlebars\Impl ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [4] {
    Property [ <default> protected $logger ]
    Property [ <default> protected $decorators ]
    Property [ <default> protected $helpers ]
    Property [ <default> protected $partials ]
  }

  - Methods [10] {
    Method [ <internal:handlebars, prototype Handlebars\Impl> public method getHelpers ] {

      - Parameters [0] {
      }
      - Return [ Handlebars\Registry or NULL ]
    }

    Method [ <internal:handlebars, prototype Handlebars\Impl> public method getPartials ] {

      - Parameters [0] {
      }
      - Return [ Handlebars\Registry or NULL ]
    }

    Method [ <internal:handlebars, prototype Handlebars\Impl> public method getDecorators ] {

      - Parameters [0] {
      }
      - Return [ Handlebars\Registry or NULL ]
    }

    Method [ <internal:handlebars, prototype Handlebars\Impl> public method getLogger ] {

      - Parameters [0] {
      }
      - Return [ Psr\Logger\LoggerInterface or NULL ]
    }

    Method [ <internal:handlebars, prototype Handlebars\Impl> public method setHelpers ] {

      - Parameters [1] {
        Parameter #0 [ <required> Handlebars\Registry $helpers ]
      }
      - Return [ Handlebars\Impl ]
    }

    Method [ <internal:handlebars, prototype Handlebars\Impl> public method setPartials ] {

      - Parameters [1] {
        Parameter #0 [ <required> Handlebars\Registry $partials ]
      }
      - Return [ Handlebars\Impl ]
    }

    Method [ <internal:handlebars, prototype Handlebars\Impl> public method setDecorators ] {

      - Parameters [1] {
        Parameter #0 [ <required> Handlebars\Registry $decorators ]
      }
      - Return [ Handlebars\Impl ]
    }

    Method [ <internal:handlebars, prototype Handlebars\Impl> public method setLogger ] {

      - Parameters [1] {
        Parameter #0 [ <required> Psr\Log\LoggerInterface $logger ]
      }
      - Return [ Handlebars\Impl ]
    }

    Method [ <internal:handlebars, inherits Handlebars\Impl> abstract public method render ] {

      - Parameters [3] {
        Parameter #0 [ <required> string $tmpl ]
        Parameter #1 [ <optional> $context ]
        Parameter #2 [ <optional> array or NULL $options ]
      }
      - Return [ string ]
    }

    Method [ <internal:handlebars, inherits Handlebars\Impl> abstract public method renderFile ] {

      - Parameters [3] {
        Parameter #0 [ <required> string $filename ]
        Parameter #1 [ <optional> $context ]
        Parameter #2 [ <optional> array or NULL $options ]
      }
      - Return [ string ]
    }
  }
}
