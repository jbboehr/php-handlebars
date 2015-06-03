--TEST--
Handlebars::escapeExpression()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Native;
use Handlebars\SafeString;
class MyString {
    public function __toString() {
         return 'MyString';
    }
}
var_dump(Native::escapeExpression(true));
var_dump(Native::escapeExpression(false));
var_dump(Native::escapeExpression(array(1, 2)));
try {
    var_dump(Native::escapeExpression(array('a' => 'b')));
} catch( \Exception $e ) {
    var_dump(get_class($e));
}
var_dump(Native::escapeExpression('string'));
var_dump(Native::escapeExpression(new MyString()));
var_dump(Native::escapeExpression(new SafeString('<>')));
var_dump(Native::escapeExpression('<>'));
--EXPECT--
string(4) "true"
string(5) "false"
string(3) "1,2"
string(27) "Handlebars\RuntimeException"
string(6) "string"
string(8) "MyString"
string(2) "<>"
string(8) "&lt;&gt;"
