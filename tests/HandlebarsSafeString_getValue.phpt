--TEST--
Handlebars\SafeString::getContent()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\SafeString;
$safeString = new SafeString('blah');
var_dump((string) $safeString->getValue());
--EXPECT--
string(4) "blah"

