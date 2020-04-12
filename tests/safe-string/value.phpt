--TEST--
Handlebars\SafeString::$value
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 70400 ) die('skip not applicable to < PHP 7.4');
?>
--FILE--
<?php
$str = new Handlebars\SafeString('foo');
$r = new ReflectionProperty($str, 'value');
$r->setAccessible(true);
try {
    $r->setValue($str, new stdClass);
} catch (TypeError $e) {
    echo $e->getMessage();
}
--EXPECT--
Typed property Handlebars\SafeString::$value must be string, stdClass used
