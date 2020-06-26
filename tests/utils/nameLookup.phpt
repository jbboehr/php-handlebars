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

var_dump(Utils::nameLookup(array('foo' => 'bar1'), 'foo'));
// casting array to object is broken on ppc64le
$obj = new stdClass;
$obj->foo = 'bar2';
var_dump(Utils::nameLookup($obj, 'foo'));
var_dump(Utils::nameLookup(new \ArrayObject(array('foo' => 'bar3')), 'foo'));
var_dump(Utils::nameLookup(new HandlebarsLookupArrayAccessFixture(array('foo' => 'bar4')), 'foo'));
var_dump(Utils::nameLookup(null, 'foo'));
// casting array to object is broken on ppc64le
$obj2 = new stdClass;
$obj2->bar = 'baz';
$obj3 = new stdClass;
$obj3->foo = $obj2;
var_dump((array) Utils::nameLookup($obj3, 'foo'));
var_dump(Utils::nameLookup(array(404 => 'bar5'), 404));
// Make sure it doesn't cause a notice
// casting array to object is broken on ppc64le
var_dump(Utils::nameLookup(new stdClass, 'missing'));
var_dump(Utils::nameLookup(new ArrayObject(), 'missing'));
var_dump(Utils::nameLookup(new HandlebarsLookupArrayAccessFixture(), 'missing'));
--EXPECT--
string(4) "bar1"
string(4) "bar2"
string(4) "bar3"
string(4) "bar4"
NULL
array(1) {
  ["bar"]=>
  string(3) "baz"
}
string(4) "bar5"
NULL
NULL
NULL
