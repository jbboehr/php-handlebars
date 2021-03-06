--TEST--
partials - partial blocks - should render nested partial blocks
--DESCRIPTION--
partials - partial blocks - should render nested partial blocks
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
$tmpl = '<template>{{#> outer}}{{value}}{{/outer}}</template>';
$context = array(
        'value' => 'success',
    );
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
        'outer' => '<outer>{{#> nested}}<outer-block>{{> @partial-block}}</outer-block>{{/nested}}</outer>',
        'nested' => '<nested>{{> @partial-block}}</nested>',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<template><outer><nested><outer-block>success</outer-block></nested></outer></template>