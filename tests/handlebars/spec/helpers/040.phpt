--TEST--
helpers - multiple parameters - simple multi-params work
--DESCRIPTION--
helpers - multiple parameters - simple multi-params work
regular helpers with multiple params
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
$tmpl = 'Message: {{goodbye cruel world}}';
$context = array(
        'cruel' => 'cruel',
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'goodbye' => function($cruel, $world) { return 'Goodbye ' . $cruel . ' ' . $world; }/*function (cruel, world) { return 'Goodbye ' + cruel + ' ' + world; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Message: Goodbye cruel world