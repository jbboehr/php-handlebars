--TEST--
Handlebars\Utils::appendContextPath()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
var_dump(Utils::appendContextPath(array('contextPath' => 'foo'), 'bar'));
var_dump(Utils::appendContextPath((object) array('contextPath' => 'foo'), 'bar'));
var_dump(Utils::appendContextPath('foo', 'bar'));
var_dump(Utils::appendContextPath(array(), 'bar'));
var_dump(Utils::appendContextPath(null, 'bar'));
var_dump(Utils::appendContextPath(array('contextPath' => 'bat'), 'field'));
--EXPECT--
string(7) "foo.bar"
string(7) "foo.bar"
string(7) "foo.bar"
string(3) "bar"
string(3) "bar"
string(9) "bat.field"
