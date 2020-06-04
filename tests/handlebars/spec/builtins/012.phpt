--TEST--
builtin helpers - #if - if with function argument - 03
--DESCRIPTION--
builtin helpers - #if - if with function argument - 03
if with function does not show the contents when returns undefined
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
$tmpl = '{{#if goodbye}}GOODBYE {{/if}}cruel {{world}}!';
$context = array(
        'goodbye' => function($scope) { return isset($scope['foo']) ? $scope['foo'] : null; }/*function () {return this.foo; }*/,
        'world' => 'world',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
cruel world!