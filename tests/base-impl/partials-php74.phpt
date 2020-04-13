--TEST--
Handlebars\BaseImpl::$partials (PHP 7.4)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 70400 || PHP_VERSION_ID >= 80000 ) die('skip ');
?>
--FILE--
<?php
require __DIR__ . "/fake-impl.php.inc";
$impl = new FakeImpl();
$r = new ReflectionProperty($impl, 'partials');
$r->setAccessible(true);
try {
    $r->setValue($impl, 'notaregistry');
} catch (TypeError $e) {
    echo $e->getMessage();
}
--EXPECT--
Typed property Handlebars\BaseImpl::$partials must be an instance of Handlebars\Registry or null, string used
