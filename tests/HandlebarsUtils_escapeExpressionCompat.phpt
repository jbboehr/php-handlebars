--TEST--
Handlebars\Utils::escapeExpressionCompat()
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
var_dump(Utils::escapeExpressionCompat(true));
var_dump(Utils::escapeExpressionCompat(false));
var_dump(Utils::escapeExpressionCompat(array(1, 2)));
try {
    var_dump(Utils::escapeExpressionCompat(array('a' => 'b')));
} catch( \Exception $e ) {
    var_dump(get_class($e));
}
var_dump(Utils::escapeExpressionCompat('string'));
var_dump(Utils::escapeExpressionCompat(new MyString()));
var_dump(Utils::escapeExpressionCompat(new SafeString('<>')));
var_dump(Utils::escapeExpressionCompat('<>'));
var_dump(Utils::escapeExpressionCompat("`'"));
var_dump(Utils::escapeExpressionCompat('"'));
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
