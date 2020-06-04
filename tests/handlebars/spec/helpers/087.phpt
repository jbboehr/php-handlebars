--TEST--
helpers - the lookupProperty-option - should be passed to custom helpers
--DESCRIPTION--
helpers - the lookupProperty-option - should be passed to custom helpers
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
$tmpl = '{{testHelper}}';
$context = array(
        'testProperty' => 'abc',
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function ($options) {
          return $options->lookupProperty($options->scope, 'testProperty');
        }/*function testHelper(options) {
          return options.lookupProperty(this, 'testProperty');
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
abc