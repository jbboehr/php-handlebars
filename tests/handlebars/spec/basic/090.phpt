--TEST--
basic context - should handle literals in subexpression
--DESCRIPTION--
basic context - should handle literals in subexpression
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
$tmpl = '{{foo (false)}}';
$context = array(
        'false' => function() { return 'bar'; }/*function () { return 'bar'; }*/,
    );
$helpers = new DefaultRegistry(array(
        'foo' => function($arg) { return $arg; }/*function (arg) { return arg; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
bar