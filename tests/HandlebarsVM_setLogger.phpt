--TEST--
Handlebars\VM::setLogger()
--EXTENSIONS--
psr
--SKIPIF--
<?php if( !extension_loaded('handlebars') || !extension_loaded('psr') ) die('skip '); ?>
--FILE--
<?php
use Psr\Log\AbstractLogger;
use Handlebars\VM;
class TestLogger extends AbstractLogger {
    public function log($level, $message, array $context = null) { var_dump($level, $message); }
}
$logger = new TestLogger();
$vm = new VM();
$vm->render('{{log "TESTING"}}');
$vm->setLogger($logger);
$vm->render('{{log "TESTING2"}}');
--EXPECT--
string(TESTING)
string(4) "info"
string(16) "string(TESTING2)"
