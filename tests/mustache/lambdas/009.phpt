--TEST--
Section - Multiple Calls - Lambdas used for sections should not be cached.
--DESCRIPTION--
Section - Multiple Calls - Lambdas used for sections should not be cached.
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
$tmpl = '{{#lambda}}FILE{{/lambda}} != {{#lambda}}LINE{{/lambda}}';
$context = array(
        'lambda' => function($text) {return "__" . $text . "__";}/*function(txt) { return "__" + txt + "__" }*/,
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
__FILE__ != __LINE__