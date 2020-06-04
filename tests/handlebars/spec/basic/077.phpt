--TEST--
basic context - this keyword nested inside helpers param - 01
--DESCRIPTION--
basic context - this keyword nested inside helpers param - 01
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
$tmpl = '{{foo [this]}}';
$context = array(
        'foo' => function($value) { return $value; }/*function (value) { return value; }*/,
        'this' => 'bar',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
bar