--TEST--
Handlebars\VM::renderFromBinaryString()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--INI--
handlebars.cache.enable=1
handlebars.cache.enable_cli=1
handlebars.cache.backend=simple
handlebars.cache.stat=0
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$binaryString = $vm->compile(file_get_contents(__DIR__ . '/../fixture1.hbs'));

var_dump($vm->renderFromBinaryString($binaryString, array('foo' => 'bar'), array(
    'cacheId' => 'bat',
)));

// not super ideal
var_dump($vm->renderFromBinaryString('', array('foo' => 'bar'), array(
    'cacheId' => 'bat',
)));

--EXPECTF--
string(3) "bar"
string(3) "bar"
