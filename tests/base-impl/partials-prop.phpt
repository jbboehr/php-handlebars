--TEST--
Handlebars\BaseImpl::$decorators
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 70400 ) die('skip not applicable to < PHP 7.4');
?>
--FILE--
<?php
require __DIR__ . "/fake-impl.php.inc";
$impl = new FakeImpl();
$r = new ReflectionProperty($impl, 'decorators');
$r->setAccessible(true);
try {
    $r->setValue($impl, 'notaregistry');
} catch (TypeError $e) {
    echo $e->getMessage();
}
--EXPECT--
Typed property Handlebars\BaseImpl::$decorators must be an instance of Handlebars\Registry or null, string used
