--TEST--
Handlebars::isIntArray()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Native;
var_dump(Native::isIntArray(true));
var_dump(Native::isIntArray(array()));
var_dump(Native::isIntArray(array('a')));
var_dump(Native::isIntArray(array('a', 'b')));
var_dump(Native::isIntArray(array('z' => 'a', 'b')));
var_dump(Native::isIntArray(array('a', 'z' => 'b')));
$tmp = array('a', 'b');
unset($tmp[0]);
var_dump(Native::isIntArray($tmp));
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
