--TEST--
Indented Inline Sections - Single-line sections should not alter surrounding whitespace.
--DESCRIPTION--
Indented Inline Sections - Single-line sections should not alter surrounding whitespace.
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
$tmpl = ' {{#boolean}}YES{{/boolean}}
 {{#boolean}}GOOD{{/boolean}}
';
$context = array(
        'boolean' => true,
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
 YES
 GOOD
