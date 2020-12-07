--TEST--
Handlebars\VM::renderFromBinaryString()
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( !function_exists('pcntl_fork') )  die('skip pcntl_fork not available');
?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$binaryString = $vm->compile(file_get_contents(__DIR__ . '/../fixture1.hbs'));
var_dump($vm->renderFromBinaryString($binaryString, array('foo' => 'bar')));

// We want the serialized buffer to be deterministic
if (version_compare(Handlebars\LIBVERSION2, "0.7.1", ">=")) {
    var_dump($binaryString === $vm->compile(file_get_contents(__DIR__ . '/../fixture1.hbs')));
} else {
    var_dump(true); // just consider this skipped
}

// Make sure a short buffer throws
try {
    $vm->renderFromBinaryString(substr($binaryString, 0, 3));
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

// Make sure an invalid hash throws
try {
    $tmp = $binaryString;
    $tmp[0] = chr(0xFF);
    $tmp[1] = chr(0xFF);
    var_dump($vm->renderFromBinaryString($tmp));
} catch (\Handlebars\InvalidBinaryStringException $e) {
    var_dump($e->getMessage());
}

// Make sure an invalid data segment throws
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

--EXPECTF--
string(%d) "bar"
bool(true)
string(%d) "Failed to validate precompiled template: buffer not long enough"
string(%d) "Failed to validate precompiled template: template hash was %s, expected %s"
string(%d) "Failed to validate precompiled template: template hash was %s, expected %s"
