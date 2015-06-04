--TEST--
Handlebars\Native::expression()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Native;
class MyString {
    public function __toString() {
         return 'MyString';
    }
}
var_dump(Native::expression(true));
var_dump(Native::expression(false));
var_dump(Native::expression(array(1, 2)));
try {
    var_dump(Native::expression(array('a' => 'b')));
} catch( \Exception $e ) {
    var_dump(get_class($e));
}
var_dump(Native::expression('string'));
var_dump(Native::expression(new MyString));
--EXPECT--
string(4) "true"
string(5) "false"
string(3) "1,2"
string(27) "Handlebars\RuntimeException"
string(6) "string"
string(8) "MyString"
