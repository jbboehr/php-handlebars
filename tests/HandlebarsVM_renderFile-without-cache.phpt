--TEST--
Handlebars\VM::render() without cache
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--INI--
handlebars.cache.enabled=0
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$tmpFile = tempnam(sys_get_temp_dir(), 'php-handlebars');
file_put_contents($tmpFile, '{{foo}}');
var_dump($vm->renderFile($tmpFile, array('foo' => 'bar')));
file_put_contents($tmpFile, '{{bar}}');
var_dump($vm->renderFile($tmpFile, array('bar' => 'baz')));
--EXPECT--
string(3) "bar"
string(3) "baz"