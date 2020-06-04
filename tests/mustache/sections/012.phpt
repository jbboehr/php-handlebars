--TEST--
Implicit Iterator - Integer - Implicit iterators should cast integers to strings and interpolate.
--DESCRIPTION--
Implicit Iterator - Integer - Implicit iterators should cast integers to strings and interpolate.
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
require __DIR__ . "/../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '"{{#list}}({{.}}){{/list}}"';
$context = array(
        'list' => array(
            1,
            2,
            3,
            4,
            5,
        ),
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
"(1)(2)(3)(4)(5)"