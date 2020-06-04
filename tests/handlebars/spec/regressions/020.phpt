--TEST--
Regressions - GH-837: undefined values for helpers
--DESCRIPTION--
Regressions - GH-837: undefined values for helpers
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
$tmpl = '{{str bar.baz}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'str' => function($value) { return $value === null ? 'undefined' : (string) $value; }/*function (value) { return value + ''; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
undefined