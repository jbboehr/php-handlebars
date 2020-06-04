--TEST--
helpers - helpers hash - providing a helpers hash
--DESCRIPTION--
helpers - helpers hash - providing a helpers hash
helpers hash is available
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
$tmpl = 'Goodbye {{cruel}} {{world}}!';
$context = array(
        'cruel' => 'cruel',
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world'; }/*function () { return 'world'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Goodbye cruel world!