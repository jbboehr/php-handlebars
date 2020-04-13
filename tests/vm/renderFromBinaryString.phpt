--TEST--
Handlebars\VM::renderFromBinaryString()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$binaryString = $vm->compile(file_get_contents(__DIR__ . '/../fixture1.hbs'));
var_dump($vm->renderFromBinaryString($binaryString, array('foo' => 'bar')));

try {
    $vm->renderFromBinaryString(substr($binaryString, 0, 3));
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

try {
    $tmp = $binaryString;
    $tmp[0] = chr(0xFF);
    $vm->renderFromBinaryString($tmp);
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

try {
    $tmp = $binaryString;
    $tmp[51] = chr(0xFF);
    $vm->renderFromBinaryString($tmp);
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

// Try one not rendered in this process
$binaryString = base64_decode("enUaJAu4pqFcAgAAAAAAAIBbkAIAAAAAcAQAAAAAAADPL5ReAAAAAAAAAAAAAAAAAQAAAAAAAADYW5ACAAAAAAgAAAAAAAAAAFyQAgAAAAAwAAAAAAAAAMBfkAIAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAAAFyQAgAAAAAAXJACAAAAAAkAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAIAAAABAAAACAAAAAoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAIAAAABAAAACAAAAAoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAIAAAABAAAACAAAAAkAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAQAAAABAAAABgAAABcAAAAAAAAABAAAAAAAAAABAAAAAAAAAMBfkAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAQAAAABAAAABgAAABQAAAAAAAAAAwAAAAAAAADcX5ACAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAIAAAABAAAACAAAAAMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAIAAAABAAAACAAAABsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMhfkAIAAAAAAwAAAAAAAACJc4gLAAAAAGZvbwADAAAAAAAAAIlziAsAAAAAZm9vAA==");
var_dump($vm->renderFromBinaryString($binaryString, array('foo' => 'bar')));

--EXPECTF--
string(%d) "bar"
string(%d) "Failed to validate precompiled template: buffer not long enough"
string(%d) "Failed to validate precompiled template: template hash was %s, expected %s"
string(%d) "Failed to validate precompiled template: template hash was %s, expected %s"
string(%d) "bar"
