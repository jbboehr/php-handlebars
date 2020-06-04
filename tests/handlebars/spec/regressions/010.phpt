--TEST--
Regressions - GS-428: Nested if else rendering - 01
--DESCRIPTION--
Regressions - GS-428: Nested if else rendering - 01
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
$tmpl = '{{#inverse}} {{#blk}} Unexpected {{/blk}} {{else}} {{#blk}} Expected {{/blk}} {{/inverse}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'blk' => function($block) { return $block/*['fn']*/->fn(''); }/*function (block) { return block.fn(''); }*/,
        'inverse' => function($block) { return $block/*['inverse']*/->inverse(''); }/*function (block) { return block.inverse(''); }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
  Expected  