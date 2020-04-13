--TEST--
Handlebars\VM::getHelpers()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\VM;
$reg = new DefaultRegistry();
$vm = new VM();
$vm->setHelpers($reg);
var_dump($vm->getHelpers() === $reg);
--EXPECT--
bool(true)
