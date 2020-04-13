--TEST--
Handlebars\Opcode::$args (PHP 7.4)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 70400 || PHP_VERSION_ID >= 80000 ) die('skip ');
?>
--FILE--
<?php
$obj = new Handlebars\Opcode('foo', ['bar']);
$r = new ReflectionProperty($obj, 'args');
$r->setAccessible(true);
try {
    $r->setValue($obj, new stdClass);
} catch (TypeError $e) {
    echo $e->getMessage();
}
--EXPECT--
Typed property Handlebars\Opcode::$args must be array, stdClass used
