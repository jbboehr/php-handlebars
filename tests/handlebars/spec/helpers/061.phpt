--TEST--
helpers - blockHelperMissing - lambdas resolved by blockHelperMissing are bound to the context
--DESCRIPTION--
helpers - blockHelperMissing - lambdas resolved by blockHelperMissing are bound to the context
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
$tmpl = '{{#truthy}}yep{{/truthy}}';
$context = array(
        'truthy' => function($options) { return $options->scope['truthiness'](); }/*function () { return this.truthiness(); }*/,
        'truthiness' => function() { return false; }/*function () { return false; }*/,
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--