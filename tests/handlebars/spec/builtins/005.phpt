--TEST--
builtin helpers - #if - if - 04
--DESCRIPTION--
builtin helpers - #if - if - 04
if with non-empty array shows the contents
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
        'goodbye' => array(
            'foo',
        ),
        'world' => 'world',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE cruel world!