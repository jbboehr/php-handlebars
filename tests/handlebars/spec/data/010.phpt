--TEST--
data - data can be functions with params
--DESCRIPTION--
data - data can be functions with params
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
$tmpl = '{{@hello "hello"}}';
$context = array();
$compileOptions = array();
$runtimeOptions = array(
        'data' => array(
            'hello' => function($arg) { return $arg; }/*function (arg) { return arg; }*/,
        ),
    );
$allOptions = array(
        'data' => array(
            'hello' => function($arg) { return $arg; }/*function (arg) { return arg; }*/,
        ),
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
hello