--TEST--
Regressions - GH-1135 : Context handling within each iteration
--DESCRIPTION--
Regressions - GH-1135 : Context handling within each iteration
--SKIPIF--
<?php
if( true ) die('skip need to implement deep equal for this?');
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
$tmpl = '{{#each array}}
 1. IF: {{#if true}}{{../name}}-{{../../name}}-{{../../../name}}{{/if}}
 2. MYIF: {{#myif true}}{{../name}}={{../../name}}={{../../../name}}{{/myif}}
{{/each}}';
$context = array(
        'array' => array(
            1,
        ),
        'name' => 'John',
    );
$helpers = new DefaultRegistry(array(
        'myif' => function($conditional, $options) {
        if ($conditional) {
          return $options->fn($options->scope);
        } else {
          return $options->inverse($options->scope);
        }
      }/*function(conditional, options) {
        if (conditional) {
          return options.fn(this);
        } else {
          return options.inverse(this);
        }
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
 1. IF: John--
 2. MYIF: John==
