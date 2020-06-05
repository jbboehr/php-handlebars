--TEST--
Handlebars\Opcode::$opcode (PHP 8)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 80000 ) die('skip ');
if( !class_exists('Handlebars\\Opcode') ) die('skip handlebars AST not enabled');
?>
--FILE--
<?php
$obj = new Handlebars\Opcode('foo', ['bar']);
$r = new ReflectionProperty($obj, 'opcode');
$r->setAccessible(true);
try {
    $r->setValue($obj, new stdClass);
} catch (TypeError $e) {
    echo $e->getMessage();
}
--EXPECT--
Cannot assign stdClass to property Handlebars\Opcode::$opcode of type string
