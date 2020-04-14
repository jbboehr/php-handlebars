--TEST--
Handlebars\VM::__construct() (without php-psr)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( extension_loaded('psr') ) die('skip ');
?>
--FILE--
<?php

namespace Psr\Log;

interface LoggerInterface {
    public function log($level, $message, array $context = null);
}
abstract class AbstractLogger implements LoggerInterface {
    public function info($message, array $context = null) { $this->log('info', $message, $context); }
    public function warning($message, array $context = null) { $this->log('warning', $message, $context); }
}

namespace IgnoreMe;

use Handlebars\DefaultRegistry;
use Handlebars\VM;
use Psr\Log\AbstractLogger;

class TestLogger extends AbstractLogger {
    public function log($level, $message, array $context = null) { var_dump($level, $message); }
}

$helpers = new DefaultRegistry();
$partials = new DefaultRegistry();
$decorators = new DefaultRegistry();
$logger = new TestLogger();
$vm = new VM(array(
    'logger' => $logger,
    'helpers' => $helpers,
    'partials' => $partials,
    'decorators' => $decorators,
));
var_dump($vm->getLogger() === $logger);
var_dump($vm->getHelpers() === $helpers);
var_dump($vm->getPartials() === $partials);
var_dump($vm->getDecorators() === $decorators);
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
