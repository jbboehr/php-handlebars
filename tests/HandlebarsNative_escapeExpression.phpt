--TEST--
Handlebars\Native::escapeExpression()
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
var_dump(Native::escapeExpression(array('a' => 'b')));
var_dump(Native::escapeExpression('string'));
var_dump(Native::escapeExpression(new MyString()));
var_dump(Native::escapeExpression(new SafeString('<>')));
var_dump(Native::escapeExpression('<>'));
var_dump(Native::escapeExpression("`'"));
var_dump(Native::escapeExpression('"'));
--EXPECTF--
string(1) "1"
string(0) ""

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"
string(6) "string"
string(8) "MyString"
string(2) "<>"
string(8) "&lt;&gt;"
string(7) "`&#039;"
string(6) "&quot;"
