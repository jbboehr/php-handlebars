--TEST--
Handlebars\Options::__construct()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Options;
$options = new Options(array(
    'name' => 'if',
    'hash' => array('a' => 'b'),
    'scope' => array('c' => 'd')
));
var_dump(get_class($options));
var_dump($options);
--EXPECT--
string(18) "Handlebars\Options"
object(Handlebars\Options)#1 (5) {
  ["fn"]=>
  NULL
  ["inverse"]=>
  NULL
  ["name"]=>
  string(2) "if"
  ["hash"]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
  ["scope"]=>
  array(1) {
    ["c"]=>
    string(1) "d"
  }
}