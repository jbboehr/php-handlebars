--TEST--
partials - partial blocks - should allow the #each-helper to be used along with partial-blocks
--DESCRIPTION--
partials - partial blocks - should allow the #each-helper to be used along with partial-blocks
--SKIPIF--
<?php
if( true ) die('skip partial blocks are not supported by the VM');
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
$tmpl = '<template>{{#> list value}}value = {{.}}{{/list}}</template>';
$context = array(
        'value' => array(
            'a',
            'b',
            'c',
        ),
    );
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
        'list' => '<list>{{#each .}}<item>{{> @partial-block}}</item>{{/each}}</list>',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<template><list><item>value = a</item><item>value = b</item><item>value = c</item></list></template>