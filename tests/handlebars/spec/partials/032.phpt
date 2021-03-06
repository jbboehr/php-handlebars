--TEST--
partials - partial blocks - should be able to render the partial-block twice
--DESCRIPTION--
partials - partial blocks - should be able to render the partial-block twice
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
$tmpl = '{{#> dude}}success{{/dude}}';
$context = array();
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
        'dude' => '{{> @partial-block }} {{> @partial-block }}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
success success