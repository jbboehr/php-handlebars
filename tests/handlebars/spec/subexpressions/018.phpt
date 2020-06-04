--TEST--
subexpressions - subexpressions can't just be property lookups
--DESCRIPTION--
subexpressions - subexpressions can't just be property lookups
--SKIPIF--
<?php
if( true ) die('skip skip for now');
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
$tmpl = '{{foo (bar)}}!';
$context = array(
        'bar' => 'LOL',
    );
$helpers = new DefaultRegistry(array(
        'foo' => function($val) {
        return $val . $val;
      }/*function (val) {
        return val + val;
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECTF--
%AUncaught%A