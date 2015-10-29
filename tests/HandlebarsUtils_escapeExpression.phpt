--TEST--
Handlebars\Utils::escapeExpression()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
use Handlebars\SafeString;
class MyString {
    public function __toString() {
         return 'MyString';
    }
}
var_dump(Utils::escapeExpression(true));
var_dump(Utils::escapeExpression(false));
var_dump(Utils::escapeExpression(array(1, 2)));
var_dump(Utils::escapeExpression(array('a' => 'b')));
var_dump(Utils::escapeExpression('string'));
var_dump(Utils::escapeExpression(new MyString()));
var_dump(Utils::escapeExpression(new SafeString('<>')));
var_dump(Utils::escapeExpression('<>'));
var_dump(Utils::escapeExpression("`'"));
var_dump(Utils::escapeExpression('"'));
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
