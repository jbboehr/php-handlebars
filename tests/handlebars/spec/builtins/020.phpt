--TEST--
builtin helpers - #each - each - 01
--DESCRIPTION--
builtin helpers - #each - each - 01
each with array argument ignores the contents when empty
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
$tmpl = '{{#each goodbyes}}{{text}}! {{/each}}cruel {{world}}!';
$context = array(
        'goodbyes' => array(),
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
cruel world!