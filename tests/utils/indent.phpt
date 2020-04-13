--TEST--
Handlebars\Utils::indent
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Utils;
var_dump(addcslashes(Utils::indent('blah', ' '), "\n"));
var_dump(addcslashes(Utils::indent("blah\nblah", ' '), "\n"));
var_dump(addcslashes(Utils::indent("\n\n\n", ' '), "\n"));
--EXPECT--
string(5) " blah"
string(12) " blah\n blah"
string(9) " \n \n \n"
