--TEST--
track ids - should note ../ and ./ references
--DESCRIPTION--
track ids - should note ../ and ./ references
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
$tmpl = '{{wycats ./is.a ../slave.driver this.is.a this}}';
$context = array(
        'is' => array(
            'a' => 'foo',
        ),
        'slave' => array(
            'driver' => 'bar',
        ),
    );
$helpers = new DefaultRegistry(array(
        'wycats' => function($passiveVoice, $noun, $thiz, $thiz2, $options) {
        return "HELP ME MY BOSS " . $options['ids'][0] . ':' . $passiveVoice . ' ' . $options['ids'][1] . ':' . ($noun === null ? 'undefined' : $noun);
      }/*function (passiveVoice, noun, thiz, thiz2, options) {
        equal(options.ids[0], 'is.a');
        equal(options.ids[1], '../slave.driver');
        equal(options.ids[2], 'is.a');
        equal(options.ids[3], '');

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
HELP ME MY BOSS is.a:foo ../slave.driver:undefined