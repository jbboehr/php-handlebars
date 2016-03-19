--TEST--
Handlebars\VM::getDecorators()
--EXTENSIONS--
psr
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\VM;
$reg = new DefaultRegistry();
$vm = new VM();
$vm->setDecorators($reg);
var_dump($vm->getDecorators() === $reg);
--EXPECT--
bool(true)
