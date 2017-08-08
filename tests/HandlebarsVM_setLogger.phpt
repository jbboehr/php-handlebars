--TEST--
Handlebars\VM::setLogger()
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
// $vm->render('{{log "TESTING"}}');
$vm->setLogger($logger);
var_dump($vm->getLogger() === $logger);
$vm->render('{{log "TESTING2"}}');
$vm->render('{{log "TESTING3" level="warning"}}');
--EXPECT--
bool(true)
string(4) "info"
string(17) "string(TESTING2) "
string(7) "warning"
string(17) "string(TESTING3) "
