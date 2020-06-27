--TEST--
MMAP Cache
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( substr(PHP_OS, 0, 3) === "WIN" ) die('skip not supported on windows');
if( Handlebars\CACHE_BACKEND != "mmap" ) die('skip mmap backend is probably disabled');
?>
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
var_dump($vm->renderFile($tmpFile, array('foo' => 'baz')));
handlebars_cache_reset();
var_dump($vm->renderFile($tmpFile, array('bar' => 'bar')));
--EXPECT--
string(4) "mmap"
string(3) "bar"
string(3) "baz"
string(3) "bar"
