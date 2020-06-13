--TEST--
Regressions - GH-1186: Support block params for existing programs
--DESCRIPTION--
Regressions - GH-1186: Support block params for existing programs
--SKIPIF--
<?php
if( true ) die('skip decorators are not supported by the VM');
?>
--FILE--
<?php
use Handlebars\Compiler;
use Handlebars\Parser;
use Handlebars\Tokenizer;
use Handlebars\Utils;
use Handlebars\VM;
require __DIR__ . "/../../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '{{#*inline "test"}}{{> @partial-block }}{{/inline}}{{#>test }}{{#each listOne as |item|}}{{ item }}{{/each}}{{/test}}{{#>test }}{{#each listTwo as |item|}}{{ item }}{{/each}}{{/test}}';
$context = array(
        'listOne' => array(
            'a',
        ),
        'listTwo' => array(
            'b',
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
ab