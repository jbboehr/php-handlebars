--TEST--
Handlebars::lookup()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Native;
class HandlebarsLookupArrayAccessFixture implements ArrayAccess {
    public function offsetGet($offset) {
         return 'Offset got: ' . $offset;
    }
    public function offsetSet($offset, $value) {
    }
    public function offsetUnset($offset) {
    }
    public function offsetExists($offset) {
    }
}
var_dump(Native::nameLookup(array('foo' => 'bar'), 'foo'));
var_dump(Native::nameLookup((object) array('foo' => 'bar'), 'foo'));
var_dump(Native::nameLookup(new \ArrayObject(array('foo' => 'bar')), 'foo'));
var_dump(Native::nameLookup(new HandlebarsLookupArrayAccessFixture(), 'foo'));
var_dump(Native::nameLookup(null, 'foo'));
var_dump((array) Native::nameLookup((object) array('foo' => (object) array('bar' => 'baz')), 'foo'));
var_dump(Native::nameLookup(array(404 => 'bar'), 404));
var_dump(Native::nameLookup(array(404 => 'bar'), '404'));
--EXPECT--
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(15) "Offset got: foo"
NULL
array(1) {
  ["bar"]=>
  string(3) "baz"
}
string(3) "bar"
string(3) "bar"
