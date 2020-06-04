--TEST--
Implicit Iterator - Array - Implicit iterators should allow iterating over nested arrays.
--DESCRIPTION--
Implicit Iterator - Array - Implicit iterators should allow iterating over nested arrays.
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
$tmpl = '"{{#list}}({{#.}}{{.}}{{/.}}){{/list}}"';
$context = array(
        'list' => array(
            array(
                1,
                2,
                3,
            ),
            array(
                'a',
                'b',
                'c',
            ),
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
"(123)(abc)"