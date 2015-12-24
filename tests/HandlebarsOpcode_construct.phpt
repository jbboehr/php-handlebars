--TEST--
Handlebars\Opcode::__construct()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Opcode;
$opcode = new Opcode('blah', array(1, 2));
var_dump(get_class($opcode));
var_dump($opcode->opcode);
var_dump($opcode->args);
--EXPECT--
string(17) "Handlebars\Opcode"
string(4) "blah"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}