--TEST--
track ids - should note @data references
--DESCRIPTION--
track ids - should note @data references
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
$tmpl = '{{wycats @is.a @slave.driver}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'wycats' => function($passiveVoice, $noun, $options) {
        return "HELP ME MY BOSS " . $options['ids'][0] . ':' . $passiveVoice . ' ' . $options['ids'][1] . ':' . $noun;
      }/*function (passiveVoice, noun, options) {
        equal(options.ids[0], '@is.a');
        equal(options.ids[1], '@slave.driver');

        return 'HELP ME MY BOSS ' + options.ids[0] + ':' + passiveVoice + ' ' + options.ids[1] + ':' + noun;
      }*/,
    ));
$compileOptions = array(
        'trackIds' => true,
    );
$runtimeOptions = array(
        'data' => array(
            'is' => array(
                'a' => 'foo',
            ),
            'slave' => array(
                'driver' => 'bar',
            ),
        ),
    );
$allOptions = array(
        'trackIds' => true,
        'data' => array(
            'is' => array(
                'a' => 'foo',
            ),
            'slave' => array(
                'driver' => 'bar',
            ),
        ),
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
HELP ME MY BOSS @is.a:foo @slave.driver:bar