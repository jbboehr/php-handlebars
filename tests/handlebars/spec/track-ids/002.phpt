--TEST--
track ids - should include argument ids
--DESCRIPTION--
track ids - should include argument ids
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
$tmpl = '{{wycats is.a slave.driver}}';
$context = array(
        'is' => array(
            'a' => 'foo',
        ),
        'slave' => array(
            'driver' => 'bar',
        ),
    );
$helpers = new DefaultRegistry(array(
        'wycats' => function($passiveVoice, $noun, $options) {
        return "HELP ME MY BOSS " . $options['ids'][0] . ':' . $passiveVoice . ' ' . $options['ids'][1] . ':' . $noun;
      }/*function (passiveVoice, noun, options) {
        equal(options.ids[0], 'is.a');
        equal(options.ids[1], 'slave.driver');

        return 'HELP ME MY BOSS ' + options.ids[0] + ':' + passiveVoice + ' ' + options.ids[1] + ':' + noun;
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
HELP ME MY BOSS is.a:foo slave.driver:bar