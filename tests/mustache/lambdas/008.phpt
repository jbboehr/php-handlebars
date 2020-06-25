--TEST--
Section - Alternate Delimiters - Lambdas used for sections should parse with the current delimiters.
--DESCRIPTION--
Section - Alternate Delimiters - Lambdas used for sections should parse with the current delimiters.
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
$tmpl = '{{= | | =}}<|#lambda|-|/lambda|>';
$context = array(
        'planet' => 'Earth',
        'lambda' => function($text) {return $text . "{{planet}} => |planet|" . $text;}/*function(txt) { return txt + "{{planet}} => |planet|" + txt }*/,
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
<-{{planet}} => Earth->