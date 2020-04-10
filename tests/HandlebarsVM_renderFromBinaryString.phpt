--TEST--
Handlebars\VM::renderFromBinaryString()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$binaryString = $vm->compile(file_get_contents(__DIR__ . '/fixture1.hbs'));
var_dump($vm->renderFromBinaryString($binaryString, array('foo' => 'bar')));

try {
    $vm->renderFromBinaryString(substr($binaryString, 0, 15));
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

try {
    $tmp = $binaryString;
    $tmp[0] = 0xFF;
    $vm->renderFromBinaryString($tmp);
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

try {
    $tmp = $binaryString;
    $tmp[80] = 0xFF;
    $vm->renderFromBinaryString($tmp);
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

--EXPECTF--
string(3) "bar"
string(73) "Failed to validate precompiled template: buffer must be at least 16 bytes"
string(134) "Failed to validate precompiled template: template hash was %s, expected %s"
string(134) "Failed to validate precompiled template: template hash was %s, expected %s"
