--TEST--
partials - partial blocks - should propagate block parameters to default block
--DESCRIPTION--
partials - partial blocks - should propagate block parameters to default block
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
$tmpl = '{{#with context as |me|}}{{#> dude}}{{me.value}}{{/dude}}{{/with}}';
$context = array(
        'context' => array(
            'value' => 'success',
        ),
    );
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