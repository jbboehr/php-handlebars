--TEST--
missing file
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
try {
    var_dump($vm->renderFile('dontreal'));
} catch (\Exception $e) {
    echo $e->getMessage(), "\n";
}
--EXPECTF--
Warning: Handlebars\VM::renderFile(dontreal): %ailed to open stream: No such file or directory in %s on line %d
Failed to read input template file
