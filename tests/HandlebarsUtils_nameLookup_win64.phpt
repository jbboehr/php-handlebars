--TEST--
Handlebars\Utils::nameLookup() weird failure on Windows x64
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
var_dump(Utils::nameLookup(array(404 => 'bar6'), '404'));
--EXPECT--
string(4) "bar6"
