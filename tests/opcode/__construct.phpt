--TEST--
Handlebars\Opcode::__construct() (PHP 8)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 80000 ) die('skip ');
if( !class_exists('Handlebars\\Opcode') ) die('skip handlebars AST not enabled');
?>
--FILE--
<?php
use Handlebars\Opcode;
$opcode = new Opcode('blah', array(1, 2));
var_dump(get_class($opcode));
var_dump($opcode->opcode);
var_dump($opcode->args);
try {
    new Opcode(new stdClass, []);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    new Opcode('blah', new stdClass);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
--EXPECT--
string(17) "Handlebars\Opcode"
string(4) "blah"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
Handlebars\Opcode::__construct(): Argument #1 ($opcode) must be of type string, stdClass given
Handlebars\Opcode::__construct(): Argument #2 ($args) must be of type array, stdClass given
