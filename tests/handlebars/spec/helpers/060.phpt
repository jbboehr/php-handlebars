--TEST--
helpers - blockHelperMissing - lambdas are resolved by blockHelperMissing, not handlebars proper
--DESCRIPTION--
helpers - blockHelperMissing - lambdas are resolved by blockHelperMissing, not handlebars proper
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
$tmpl = '{{#truthy}}yep{{/truthy}}';
$context = array(
        'truthy' => function() {
        return true;
      }/*function () {
        return true;
      }*/,
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
yep