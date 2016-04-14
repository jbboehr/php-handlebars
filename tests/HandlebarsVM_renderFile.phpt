--TEST--
Handlebars\VM::renderFile()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
for( $i = 0; $i < 2; $i++ ) {
    var_dump($vm->renderFile(__DIR__ . '/fixture1.hbs', array('foo' => 'bar')));
}
--EXPECT--
string(3) "bar"
string(3) "bar"