--TEST--
Standalone Indentation - Each line of the partial should be indented before rendering.
--DESCRIPTION--
Standalone Indentation - Each line of the partial should be indented before rendering.
--SKIPIF--
<?php
if( true ) die('skip skip for now');
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
$tmpl = '\\
 {{>partial}}
/
';
$context = array(
        'content' => '<' . "\n" . '->',
    );
$partials = new DefaultRegistry(array(
        'partial' => '|' . "\n" . '{{{content}}}' . "\n" . '|' . "\n" . '',
    ));
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
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
\
 |
 <
->
 |
/
