--TEST--
helpers - helpers hash - the helper hash should augment the global hash
--DESCRIPTION--
helpers - helpers hash - the helper hash should augment the global hash
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
$tmpl = '{{test_helper}} {{#if cruel}}Goodbye {{cruel}} {{world}}!{{/if}}';
$context = array(
        'cruel' => 'cruel',
    );
$helpers = new DefaultRegistry(array(
        'test_helper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
found it! Goodbye cruel world!!