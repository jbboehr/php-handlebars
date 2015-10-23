--TEST--
Handlebars\Utils::nameLookup()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
class HandlebarsLookupArrayAccessFixture implements ArrayAccess {
    protected $array;
    public function __construct(array $array = array()) {
        $this->array = $array;
    }
    public function offsetGet($offset) {
        return $this->array[$offset];
    }
    public function offsetSet($offset, $value) {
    }
    public function offsetUnset($offset) {
    }
    public function offsetExists($offset) {
        return isset($this->array[$offset]);
    }
}
error_reporting(E_ALL);

var_dump(Utils::nameLookup(array('foo' => 'bar'), 'foo'));
var_dump(Utils::nameLookup((object) array('foo' => 'bar'), 'foo'));
var_dump(Utils::nameLookup(new \ArrayObject(array('foo' => 'bar')), 'foo'));
var_dump(Utils::nameLookup(new HandlebarsLookupArrayAccessFixture(array('foo' => 'bar')), 'foo'));
var_dump(Utils::nameLookup(null, 'foo'));
var_dump((array) Utils::nameLookup((object) array('foo' => (object) array('bar' => 'baz')), 'foo'));
var_dump(Utils::nameLookup(array(404 => 'bar'), 404));
var_dump(Utils::nameLookup(array(404 => 'bar'), '404'));
// Make sure it doesn't cause a notice
var_dump(Utils::nameLookup((object) array(), 'missing'));
var_dump(Utils::nameLookup(new ArrayObject(), 'missing'));
var_dump(Utils::nameLookup(new HandlebarsLookupArrayAccessFixture(), 'missing'));
--EXPECT--
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
NULL
array(1) {
  ["bar"]=>
  string(3) "baz"
}
string(3) "bar"
string(3) "bar"
NULL
NULL
NULL
