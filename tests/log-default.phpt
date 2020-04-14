--TEST--
default log function
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$vm->render('{{log "TESTING"}}');
--EXPECT--
string(TESTING)
