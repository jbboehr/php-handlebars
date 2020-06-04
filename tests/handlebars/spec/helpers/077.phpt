--TEST--
helpers - block params - should take presednece over parent block params
--DESCRIPTION--
helpers - block params - should take presednece over parent block params
--SKIPIF--
<?php
if( 1 ) die('skip libhandlebars currently does not expose options.blockParams');
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
$tmpl = '{{#goodbyes as |value|}}{{#goodbyes}}{{value}}{{#goodbyes as |value|}}{{value}}{{/goodbyes}}{{/goodbyes}}{{/goodbyes}}{{value}}';
$context = array(
        'value' => 'foo',
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'goodbyes' => function($options) {
             static $value = 0;
          return $options->fn(
            [ "value" => 'bar' ],
            [
                 // blockParams probably wrong
              "blockParams" => $options->blockParams === 1 ? [$value++, $value++] : null
            ]
          );
        }/*function (options) {
          if( typeof value === 'undefined' ) { value = 1; } return options.fn({value: 'bar'}, {blockParams: options.fn.blockParams === 1 ? [global.value++, global.value++] : undefined});
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
13foo