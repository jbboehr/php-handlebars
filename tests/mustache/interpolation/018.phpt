--TEST--
Dotted Names - Arbitrary Depth - Dotted names should be functional to any level of nesting.
--DESCRIPTION--
Dotted Names - Arbitrary Depth - Dotted names should be functional to any level of nesting.
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
$tmpl = '"{{a.b.c.d.e.name}}" == "Phil"';
$context = array(
        'a' => array(
            'b' => array(
                'c' => array(
                    'd' => array(
                        'e' => array(
                            'name' => 'Phil',
                        ),
                    ),
                ),
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
"Phil" == "Phil"