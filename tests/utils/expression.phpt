--TEST--
Handlebars\Utils::expression()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
class MyString {
    public function __toString() {
         return 'MyString';
    }
}
var_dump(Utils::expression(true));
var_dump(Utils::expression(false));
var_dump(Utils::expression(array(1, 2)));
try {
    var_dump(Utils::expression(array('a' => 'b')));
} catch( \Exception $e ) {
    var_dump(get_class($e));
}
var_dump(Utils::expression('string'));
var_dump(Utils::expression(new MyString));
try {
    var_dump(Utils::expression((object) array('a' => 'b')));
} catch( \Exception $e ) {
    var_dump(get_class($e));
}
--EXPECT--
string(4) "true"
string(5) "false"
string(3) "1,2"
string(27) "Handlebars\RuntimeException"
string(6) "string"
string(8) "MyString"
string(27) "Handlebars\RuntimeException"
