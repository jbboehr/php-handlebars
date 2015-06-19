--TEST--
Handlebars\Native::isCallable()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Native;
class IsCallableFixture1 {
    public function __invoke() {}
}
var_dump(Native::isCallable(true));
var_dump(Native::isCallable(123));
var_dump(Native::isCallable('notafunction'));
var_dump(Native::isCallable('mail'));
var_dump(Native::isCallable(array('\\Handlebars\\Native', 'isCallable')));
var_dump(Native::isCallable(array('NotARealClass', 'NotARealFunction')));
var_dump(Native::isCallable(new IsCallableFixture1()));
var_dump(Native::isCallable(new Exception('meh')));
var_dump(Native::isCallable(function() {}));
var_dump(Native::isCallable(array(new Exception('meh'), 'getMessage')));
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)