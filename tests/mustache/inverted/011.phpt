--TEST--
Dotted Names - Falsey - Dotted names should be valid for Inverted Section tags.
--DESCRIPTION--
Dotted Names - Falsey - Dotted names should be valid for Inverted Section tags.
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
$tmpl = '"{{^a.b.c}}Not Here{{/a.b.c}}" == "Not Here"';
$context = array(
        'a' => array(
            'b' => array(
                'c' => false,
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
"Not Here" == "Not Here"