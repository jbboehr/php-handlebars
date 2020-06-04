--TEST--
string params mode - with nested block ambiguous
--DESCRIPTION--
string params mode - with nested block ambiguous
--SKIPIF--
<?php
if( true ) die('skip string params are not supported by the VM');
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
$tmpl = '{{#with content}}{{#view}}{{firstName}} {{lastName}}{{/view}}{{/with}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'with' => function($options) {
        return "WITH";
      }/*function () {
        return 'WITH';
      }*/,
        'view' => function() {
        return "VIEW";
      }/*function () {
        return 'VIEW';
      }*/,
    ));
$compileOptions = array(
        'stringParams' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'stringParams' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
WITH