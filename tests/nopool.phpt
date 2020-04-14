--TEST--
render without a pool
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--INI--
handlebars.pool_size=0
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
var_dump($vm->render('{{foo}}', ['foo' => 'bar']));
--EXPECT--
string(3) "bar"
