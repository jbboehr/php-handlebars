--TEST--
basic context - depthed block functions with context argument
--DESCRIPTION--
basic context - depthed block functions with context argument
block functions are called with context and options
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
$tmpl = '{{#with value}}{{#../awesome 1}}inner {{.}}{{/../awesome}}{{/with}}';
$context = array(
        'value' => true,
        'awesome' => function($context, $options) { return $options->fn($context); }/*function (context, options) { return options.fn(context); }*/,
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
inner 1