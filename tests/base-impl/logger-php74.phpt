--TEST--
Handlebars\BaseImpl::$logger (PHP 7.4)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 70400 || PHP_VERSION_ID >= 80000 ) die('skip ');
if( !extension_loaded('psr') ) die('skip needs psr extension');
?>
--FILE--
<?php
require __DIR__ . "/fake-impl.php.inc";
$impl = new FakeImpl();
$r = new ReflectionProperty($impl, 'logger');
$r->setAccessible(true);
try {
    $r->setValue($impl, 'notaregistry');
} catch (TypeError $e) {
    echo $e->getMessage();
}
--EXPECT--
Typed property Handlebars\BaseImpl::$logger must be an instance of Psr\Log\LoggerInterface or null, string used
