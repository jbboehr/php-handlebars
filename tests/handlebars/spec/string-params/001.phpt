--TEST--
string params mode - arguments to helpers can be retrieved from options hash in string form
--DESCRIPTION--
string params mode - arguments to helpers can be retrieved from options hash in string form
String parameters output
--SKIPIF--
<?php
if( true ) die('skip string params are not supported by the VM');
?>
--FILE--
<?php
use Handlebars\Compiler;
use Handlebars\Parser;
use Handlebars\Tokenizer;
use Handlebars\Utils;
use Handlebars\VM;
require __DIR__ . "/../../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '{{wycats is.a slave.driver}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'wycats' => function($passiveVoice, $noun) {
        return "HELP ME MY BOSS " . $passiveVoice . ' ' . $noun;
      }/*function (passiveVoice, noun) {
        return 'HELP ME MY BOSS ' + passiveVoice + ' ' + noun;
      }*/,
    ));
$compileOptions = array(
        'stringParams' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'stringParams' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
HELP ME MY BOSS is.a slave.driver