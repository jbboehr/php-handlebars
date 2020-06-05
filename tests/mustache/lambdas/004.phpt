--TEST--
Interpolation - Multiple Calls - Interpolated lambdas should not be cached.
--DESCRIPTION--
Interpolation - Multiple Calls - Interpolated lambdas should not be cached.
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( !defined('Handlebars\Compiler::MUSTACHE_STYLE_LAMBDAS') ) die('skip configured libhandlebars version has no lambda support');
?>
--FILE--
<?php
use Handlebars\Compiler;
use Handlebars\Parser;
use Handlebars\Tokenizer;
use Handlebars\Utils;
use Handlebars\VM;
require __DIR__ . "/../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '{{lambda}} == {{{lambda}}} == {{lambda}}';
$context = array(
        'lambda' => function($text) {global $calls; return ++$calls;}/*function() { return (g=(function(){return this})()).calls=(g.calls||0)+1 }*/,
    );
$compileOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
1 == 2 == 3