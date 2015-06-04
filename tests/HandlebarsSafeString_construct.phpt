--TEST--
Handlebars\SafeString::__construct()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\SafeString;
$safeString = new SafeString('blah');
var_dump(get_class($safeString));
$r = new ReflectionProperty($safeString, 'value');
$r->setAccessible(true);
var_dump($r->getValue($safeString));
--EXPECT--
string(21) "Handlebars\SafeString"
string(4) "blah"
