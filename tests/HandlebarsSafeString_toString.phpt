--TEST--
Handlebars\SafeString::__toString()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\SafeString;
$safeString = new SafeString('blah');
var_dump((string) $safeString);
--EXPECT--
string(4) "blah"

