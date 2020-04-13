--TEST--
context reference
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--INI--
handlebars.pool_size=0
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$dataReal = ['foo' => 'bar'];
$dataRef = &$dataReal;
var_dump($vm->render('{{foo}}', $dataRef));
--EXPECT--
string(3) "bar"
