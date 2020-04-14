--TEST--
Handlebars\Utils::createFrame()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
$arr1 = array('foo' => 'bar');
$arr2 = Utils::createFrame($arr1);
var_dump($arr2);
var_dump($arr1 === $arr2);
var_dump($arr1 === $arr2['_parent']);

$scalar = 'baz';
$arr3 = Utils::createFrame($scalar);
var_dump($arr3);
--EXPECT--
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["_parent"]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}
bool(false)
bool(true)
array(1) {
  [0]=>
  string(3) "baz"
}
