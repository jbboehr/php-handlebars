--TEST--
builtin helpers - #if - if - 05
--DESCRIPTION--
builtin helpers - #if - if - 05
if with empty array does not show the contents
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
$tmpl = '{{#if goodbye}}GOODBYE {{/if}}cruel {{world}}!';
$context = array(
        'goodbye' => array(),
        'world' => 'world',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
cruel world!