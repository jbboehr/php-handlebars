--TEST--
Interpolation - Alternate Delimiters - A lambda's return value should parse with the default delimiters.
--DESCRIPTION--
Interpolation - Alternate Delimiters - A lambda's return value should parse with the default delimiters.
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
$tmpl = '{{= | | =}}
Hello, (|&lambda|)!';
$context = array(
        'planet' => 'world',
        'lambda' => function($text) {return "|planet| => {{planet}}";}/*function() { return "|planet| => {{planet}}" }*/,
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
Hello, (|planet| => world)!