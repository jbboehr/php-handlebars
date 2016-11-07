--TEST--
Test cache nostat
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--INI--
handlebars.cache.enable=1
handlebars.cache.enable_cli=1
handlebars.cache.backend=mmap
handlebars.cache.stat=0
--FILE--
<?php
use Handlebars\VM;
var_dump(Handlebars\CACHE_BACKEND);
$vm = new VM();
$tmpFile = tempnam(sys_get_temp_dir(), 'php-handlebars');
file_put_contents($tmpFile, '{{foo}}');
var_dump($vm->renderFile($tmpFile, array('foo' => 'bar')));
file_put_contents($tmpFile, '{{bar}}');
touch($tmpFile, time() + 1);
clearstatcache();
var_dump($vm->renderFile($tmpFile, array('foo' => 'baz')));
--EXPECT--
string(4) "mmap"
string(3) "bar"
string(3) "baz"
