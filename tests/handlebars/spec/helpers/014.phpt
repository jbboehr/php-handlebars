--TEST--
helpers - block helper
--DESCRIPTION--
helpers - block helper
Block helper executed
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
$tmpl = '{{#goodbyes}}{{text}}! {{/goodbyes}}cruel {{world}}!';
$context = array(
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'goodbyes' => function($options) { return $options->fn(array('text' => 'GOODBYE')); }/*function (options) { return options.fn({text: 'GOODBYE'}); }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE! cruel world!