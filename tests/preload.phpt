--TEST--
preload
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
var_dump(Handlebars\CACHE_BACKEND);
$vm = new VM();
$tmpFile = tempnam(sys_get_temp_dir(), 'php-handlebars');
file_put_contents($tmpFile, '{{foo}}');

var_dump($vm->preloadFile($tmpFile));
unlink($tmpFile);
var_dump($vm->renderFile($tmpFile, array('foo' => 'bar')));

var_dump($vm->preload('{{foo}}'));
var_dump($vm->render('{{foo}}', array('foo' => 'bar')));


--EXPECT--
string(6) "simple"
bool(true)
string(3) "bar"
bool(true)
string(3) "bar"
