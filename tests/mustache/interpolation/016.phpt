--TEST--
Dotted Names - Triple Mustache Interpolation - Dotted names should be considered a form of shorthand for sections.
--DESCRIPTION--
Dotted Names - Triple Mustache Interpolation - Dotted names should be considered a form of shorthand for sections.
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
$tmpl = '"{{{person.name}}}" == "{{#person}}{{{name}}}{{/person}}"';
$context = array(
        'person' => array(
            'name' => 'Joe',
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
"Joe" == "Joe"