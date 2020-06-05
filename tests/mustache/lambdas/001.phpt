--TEST--
Interpolation - A lambda's return value should be interpolated.
--DESCRIPTION--
Interpolation - A lambda's return value should be interpolated.
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
$tmpl = 'Hello, {{lambda}}!';
$context = array(
        'lambda' => function($text) {return "world";}/*function() { return "world" }*/,
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
Hello, world!