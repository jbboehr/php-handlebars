--TEST--
helpers - block params - should allow block params on chained helpers
--DESCRIPTION--
helpers - block params - should allow block params on chained helpers
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
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
$tmpl = '{{#if bar}}{{else goodbyes as |value|}}{{value}}{{/if}}{{value}}';
$context = array(
        'value' => 'foo',
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'goodbyes' => function($options) {
          return $options->fn(array('value' => 'bar'), array('blockParams' => array(1, 2)));
        }/*function (options) {
          equals(options.fn.blockParams, 1);
          return options.fn({value: 'bar'}, {blockParams: [1, 2]});
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
1foo