--TEST--
basic context - depthed functions with context argument
--DESCRIPTION--
basic context - depthed functions with context argument
functions are called with context arguments
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
$tmpl = '{{#with frank}}{{../awesome .}}{{/with}}';
$context = array(
        'awesome' => function($context) { return $context; }/*function (context) { return context; }*/,
        'frank' => 'Frank',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Frank