--TEST--
partials - partial blocks - should render block from partial with context - 01
--DESCRIPTION--
partials - partial blocks - should render block from partial with context - 01
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
$tmpl = '{{#> dude}}{{../context/value}}{{/dude}}';
$context = array(
        'context' => array(
            'value' => 'success',
        ),
    );
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
        'dude' => '{{#with context}}{{> @partial-block }}{{/with}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
success