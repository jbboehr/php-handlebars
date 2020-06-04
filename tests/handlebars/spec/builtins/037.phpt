--TEST--
builtin helpers - #each - each object when last key is an empty string
--DESCRIPTION--
builtin helpers - #each - each object when last key is an empty string
Empty string key is not skipped
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
$tmpl = '{{#each goodbyes}}{{@index}}. {{text}}! {{/each}}cruel {{world}}!';
$context = array(
        'goodbyes' => array(
            'a' => array(
                'text' => 'goodbye',
            ),
            'b' => array(
                'text' => 'Goodbye',
            ),
            '' => array(
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
0. goodbye! 1. Goodbye! 2. GOODBYE! cruel world!