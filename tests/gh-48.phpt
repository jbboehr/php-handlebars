--TEST--
GH #48 - lookup segfault
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
var_dump($vm->render('{{lookup a null}}', array(
    'a' => array('c' => null),
)));
--EXPECT--
string(0) ""
