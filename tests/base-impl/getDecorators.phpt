--TEST--
Handlebars\BaseImpl::getDecorators()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
require __DIR__ . "/fake-impl.php.inc";
$reg = new DefaultRegistry();
$impl = new FakeImpl();
$impl->setDecorators($reg);
var_dump($impl->getDecorators() === $reg);
--EXPECT--
bool(true)
