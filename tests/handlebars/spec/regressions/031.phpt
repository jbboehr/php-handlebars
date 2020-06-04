--TEST--
Regressions - GH-1341: 4.0.7 release breaks {{#if @partial-block}} usage
--DESCRIPTION--
Regressions - GH-1341: 4.0.7 release breaks {{#if @partial-block}} usage
--SKIPIF--
<?php
if( true ) die('skip partial blocks are not supported by the VM');
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
$tmpl = 'template {{>partial}} template';
$context = array();
$partials = new DefaultRegistry(array(
        'partialWithBlock' => '{{#if @partial-block}} block {{> @partial-block}} block {{/if}}',
        'partial' => '{{#> partialWithBlock}} partial {{/partialWithBlock}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
template  block  partial  block  template