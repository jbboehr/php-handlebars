--TEST--
helpers - knownHelpers - Unknown helper call in knownHelpers only mode should throw
--DESCRIPTION--
helpers - knownHelpers - Unknown helper call in knownHelpers only mode should throw
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
$tmpl = '{{typeof hello}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
    ));
$compileOptions = array(
        'knownHelpersOnly' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'knownHelpersOnly' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECTF--
%AUncaught%A