--TEST--
Implicit Iterator - String - Implicit iterators should directly interpolate strings.
--DESCRIPTION--
Implicit Iterator - String - Implicit iterators should directly interpolate strings.
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
            'a',
            'b',
            'c',
            'd',
            'e',
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
"(a)(b)(c)(d)(e)"