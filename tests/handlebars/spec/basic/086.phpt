--TEST--
basic context - pass number literals - 04
--DESCRIPTION--
basic context - pass number literals - 04
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
$tmpl = '{{12.34 1}}';
$context = array(
        '12.34' => function($arg) { return 'bar' . $arg; }/*function (arg) { return 'bar' + arg; }*/,
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
bar1