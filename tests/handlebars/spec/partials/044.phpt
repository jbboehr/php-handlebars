--TEST--
partials - inline partials - should overwrite multiple partials in the same template
--DESCRIPTION--
partials - inline partials - should overwrite multiple partials in the same template
--SKIPIF--
<?php
if( true ) die('skip inline partials are not supported by the VM');
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
$tmpl = '{{#*inline "myPartial"}}fail{{/inline}}{{#*inline "myPartial"}}success{{/inline}}{{> myPartial}}';
$context = array();
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
success