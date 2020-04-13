--TEST--
Handlebars\Utils::isCallable()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
class IsCallableFixture1 {
    public function __invoke() {}
}
var_dump(Utils::isCallable(true));
var_dump(Utils::isCallable(123));
var_dump(Utils::isCallable('notafunction'));
var_dump(Utils::isCallable('mail'));
var_dump(Utils::isCallable(array('\\Handlebars\\Utils', 'isCallable')));
var_dump(Utils::isCallable(array('NotARealClass', 'NotARealFunction')));
var_dump(Utils::isCallable(new IsCallableFixture1()));
var_dump(Utils::isCallable(new Exception('meh')));
var_dump(Utils::isCallable(function() {}));
var_dump(Utils::isCallable(array(new Exception('meh'), 'getMessage')));
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
