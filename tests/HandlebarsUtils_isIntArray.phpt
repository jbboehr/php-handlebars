--TEST--
Handlebars\Utils::isIntArray()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
var_dump(Utils::isIntArray(true));
var_dump(Utils::isIntArray(array()));
var_dump(Utils::isIntArray(array('a')));
var_dump(Utils::isIntArray(array('a', 'b')));
var_dump(Utils::isIntArray(array('z' => 'a', 'b')));
var_dump(Utils::isIntArray(array('a', 'z' => 'b')));
$tmp = array('a', 'b');
unset($tmp[0]);
var_dump(Utils::isIntArray($tmp));
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
