--TEST--
partials - GH-14: a partial preceding a selector
--DESCRIPTION--
partials - GH-14: a partial preceding a selector
Regular selectors can follow a partial
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
$tmpl = 'Dudes: {{>dude}} {{anotherDude}}';
$context = array(
        'name' => 'Jeepers',
        'anotherDude' => 'Creepers',
    );
$partials = new DefaultRegistry(array(
        'dude' => '{{name}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Dudes: Jeepers Creepers