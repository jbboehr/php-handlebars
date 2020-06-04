--TEST--
basic context - pathed block functions without context argument
--DESCRIPTION--
basic context - pathed block functions without context argument
block functions are called with options
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
$tmpl = '{{#foo.awesome}}inner{{/foo.awesome}}';
$context = array(
        'foo' => array(
            'awesome' => function($context) { return $context; }/*function () { return this; }*/,
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
inner