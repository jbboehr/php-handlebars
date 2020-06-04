--TEST--
builtin helpers - #each - each with nested @index
--DESCRIPTION--
builtin helpers - #each - each with nested @index
The @index variable is used
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
require __DIR__ . "/../../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '{{#each goodbyes}}{{@index}}. {{text}}! {{#each ../goodbyes}}{{@index}} {{/each}}After {{@index}} {{/each}}{{@index}}cruel {{world}}!';
$context = array(
        'goodbyes' => array(
            array(
                'text' => 'goodbye',
            ),
            array(
                'text' => 'Goodbye',
            ),
            array(
                'text' => 'GOODBYE',
            ),
        ),
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'detectDataInsideEach' => function($options) { return isset($options['data']['exclaim'])? $options['data']['exclaim'] : ''; }/*function(options) { return options.data && options.data.exclaim; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
0. goodbye! 0 1 2 After 0 1. Goodbye! 0 1 2 After 1 2. GOODBYE! 0 1 2 After 2 cruel world!