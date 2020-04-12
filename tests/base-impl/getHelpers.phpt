--TEST--
Handlebars\BaseImpl::getHelpers()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
require __DIR__ . "/fake-impl.php.inc";
$reg = new DefaultRegistry();
$impl = new FakeImpl();
$impl->setHelpers($reg);
var_dump($impl->getHelpers() === $reg);
--EXPECT--
bool(true)
