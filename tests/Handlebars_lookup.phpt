--TEST--
Handlebars::lookup()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Native;
var_dump(Native::nameLookup(array('foo' => 'bar'), 'foo'));
var_dump(Native::nameLookup((object) array('foo' => 'bar'), 'foo'));
var_dump(Native::nameLookup(new \ArrayObject(array('foo' => 'bar')), 'foo'));
var_dump(Native::nameLookup(null, 'foo'));
var_dump((array) Native::nameLookup((object) array('foo' => (object) array('bar' => 'baz')), 'foo'));
--EXPECT--
string(3) "bar"
string(3) "bar"
string(3) "bar"
NULL
array(1) {
  ["bar"]=>
  string(3) "baz"
}