--TEST--
Inverted Section - Lambdas used for inverted sections should be considered truthy.
--DESCRIPTION--
Inverted Section - Lambdas used for inverted sections should be considered truthy.
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
$tmpl = '<{{^lambda}}{{static}}{{/lambda}}>';
$context = array(
        'static' => 'static',
        'lambda' => function($text) {return false;}/*function(txt) { return false }*/,
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
<>