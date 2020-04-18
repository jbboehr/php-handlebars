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
    $tmp[1] = chr(0xFF);
    var_dump($vm->renderFromBinaryString($tmp));
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

try {
    $tmp = $binaryString;
    // Hitting only one random bit seems to fail (pass) once in a while
    for ($i = 51; $i < 251; $i++) {
        $tmp[$i] = chr(0xFF);
    }
    var_dump($vm->renderFromBinaryString($tmp));
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

// Try one not compiled in this process - make sure the serialized pointers are to invalid memory
$tmpfile = tempnam(sys_get_temp_dir(), 'php-handlebars-rfbs');
$pid = pcntl_fork();
if ($pid == -1) {
    die('could not fork');
} else if (!$pid) {
    // child process
    file_put_contents($tmpfile, base64_encode($vm->compile(file_get_contents(__DIR__ . '/../fixture1.hbs'))));
    exit(0);
}
pcntl_wait($status);
var_dump($vm->renderFromBinaryString(base64_decode(file_get_contents($tmpfile)), ['foo' => 'bar']));

--EXPECTF--
string(%d) "bar"
string(%d) "Failed to validate precompiled template: buffer not long enough"
string(%d) "Failed to validate precompiled template: template hash was %s, expected %s"
string(%d) "Failed to validate precompiled template: template hash was %s, expected %s"
string(%d) "bar"
