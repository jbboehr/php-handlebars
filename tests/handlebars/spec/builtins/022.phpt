--TEST--
builtin helpers - #each - each without data - 01
--DESCRIPTION--
builtin helpers - #each - each without data - 01
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
$tmpl = '{{#each .}}{{.}}{{/each}}';
$context = array(
        'goodbyes' => 'cruel',
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'detectDataInsideEach' => function($options) { return isset($options['data']['exclaim'])? $options['data']['exclaim'] : ''; }/*function(options) { return options.data && options.data.exclaim; }*/,
    ));
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array(
        'data' => false,
    );
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
cruelworld