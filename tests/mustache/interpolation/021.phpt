--TEST--
Dotted Names - Initial Resolution - The first part of a dotted name should resolve as any other name.
--DESCRIPTION--
Dotted Names - Initial Resolution - The first part of a dotted name should resolve as any other name.
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
$tmpl = '"{{#a}}{{b.c.d.e.name}}{{/a}}" == "Phil"';
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
        'b' => array(
            'c' => array(
                'd' => array(
                    'e' => array(
                        'name' => 'Wrong',
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