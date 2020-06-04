--TEST--
track ids - should use block param paths
--DESCRIPTION--
track ids - should use block param paths
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
$tmpl = '{{#doIt as |is|}}{{wycats is.a slave.driver is}}{{/doIt}}';
$context = array(
        'is' => array(
            'a' => 'foo',
        ),
        'slave' => array(
            'driver' => 'bar',
        ),
    );
$helpers = new DefaultRegistry(array(
        'doIt' => function($options) {
        $blockParams = array($options->scope['is']);
        $blockParams['path'] = array('zomg');
        return $options->fn($options->scope, array('blockParams' => $blockParams));
      }/*function (options) {
        var blockParams = [this.is];
        blockParams.path = ['zomg'];
        return options.fn(this, {blockParams: blockParams});
      }*/,
        'wycats' => function($passiveVoice, $noun, $blah, $options) {
        return 'HELP ME MY BOSS ' . $options->ids[0] . ':' . $passiveVoice . ' ' . $options->ids[1] . ':' . $noun;
      }/*function (passiveVoice, noun, blah, options) {
        equal(options.ids[0], 'zomg.a');
        equal(options.ids[1], 'slave.driver');
        equal(options.ids[2], 'zomg');

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
HELP ME MY BOSS zomg.a:foo slave.driver:bar