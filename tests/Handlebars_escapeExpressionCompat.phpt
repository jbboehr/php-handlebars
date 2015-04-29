--TEST--
Handlebars::escapeExpressionCompat()
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
var_dump(Native::escapeExpressionCompat(true));
var_dump(Native::escapeExpressionCompat(false));
var_dump(Native::escapeExpressionCompat(array(1, 2)));
try {
    var_dump(Native::escapeExpressionCompat(array('a' => 'b')));
} catch( \Exception $e ) {
    var_dump(get_class($e));
}
var_dump(Native::escapeExpressionCompat('string'));
var_dump(Native::escapeExpressionCompat(new MyString()));
var_dump(Native::escapeExpressionCompat(new SafeString('<>')));
var_dump(Native::escapeExpressionCompat('<>'));
var_dump(Native::escapeExpressionCompat("`'"));
var_dump(Native::escapeExpressionCompat('"'));
--EXPECT--
string(4) "true"
string(5) "false"
string(3) "1,2"
string(27) "Handlebars\RuntimeException"
string(6) "string"
string(8) "MyString"
string(2) "<>"
string(8) "&lt;&gt;"
string(12) "&#x60;&#x27;"
string(6) "&quot;"