--TEST--
track ids - should return true for subexpressions
--DESCRIPTION--
track ids - should return true for subexpressions
--SKIPIF--
<?php
if( true ) die('skip track ids are not supported by the VM');
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
$tmpl = '{{wycats (sub)}}';
$context = array(
        'is' => array(
            'a' => 'foo',
        ),
        'slave' => array(
            'driver' => 'bar',
        ),
    );
$helpers = new DefaultRegistry(array(
        'sub' => function() { return 1; }/*function () { return 1; }*/,
        'wycats' => function($passiveVoice, $options) {
        return "HELP ME MY BOSS " . $passiveVoice;
      }/*function (passiveVoice, options) {
        equal(options.ids[0], true);

        return 'HELP ME MY BOSS ' + passiveVoice;
      }*/,
    ));
$compileOptions = array(
        'trackIds' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'trackIds' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
HELP ME MY BOSS 1