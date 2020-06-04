--TEST--
track ids - should return null for constants
--DESCRIPTION--
track ids - should return null for constants
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
$tmpl = '{{wycats 1 "foo" key=false}}';
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
        return "HELP ME MY BOSS " . $passiveVoice . ' ' . $noun . ' ' . ($options['hash']['key'] === false ? 'false' : $options['hash']['key']);
      }/*function (passiveVoice, noun, options) {
        equal(options.ids[0], null);
        equal(options.ids[1], null);
        equal(options.hashIds.key, null);

        return 'HELP ME MY BOSS ' + passiveVoice + ' ' + noun + ' ' + options.hash.key;
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
HELP ME MY BOSS 1 foo false