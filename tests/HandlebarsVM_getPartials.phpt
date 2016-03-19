--TEST--
Handlebars\VM::getPartials()
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
$vm->setPartials($reg);
var_dump($vm->getPartials() === $reg);
--EXPECT--
bool(true)
