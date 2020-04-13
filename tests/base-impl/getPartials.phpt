--TEST--
Handlebars\BaseImpl::getPartials()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
require __DIR__ . "/fake-impl.php.inc";
$reg = new DefaultRegistry();
$impl = new FakeImpl();
$impl->setPartials($reg);
var_dump($impl->getPartials() === $reg);
--EXPECT--
bool(true)
