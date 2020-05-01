--TEST--
Handlebars\VM::renderFromBinaryString() fork
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( !function_exists('pcntl_fork') ) die('skip ');
?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();

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
