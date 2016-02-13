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
object(Handlebars\Options)#1 (16) {
  ["name"]=>
  string(2) "if"
  ["hash"]=>
  array(1) {
    ["a"]=>
    string(1) "b"
  }
  ["fn"]=>
  NULL
  ["inverse"]=>
  NULL
  ["scope"]=>
  array(1) {
    ["c"]=>
    string(1) "d"
  }
  ["data"]=>
  NULL
  ["ids"]=>
  NULL
  ["hashIds"]=>
  NULL
  ["hashTypes"]=>
  NULL
  ["hashContexts"]=>
  NULL
  ["types"]=>
  NULL
  ["contexts"]=>
  NULL
  ["args"]=>
  NULL
  ["partial"]=>
  NULL
  ["programGuid":"Handlebars\Options":private]=>
  int(-1)
  ["inverseGuid":"Handlebars\Options":private]=>
  int(-1)
}
