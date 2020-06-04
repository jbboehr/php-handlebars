--TEST--
whitespace control - blocks - should strip whitespace around simple block calls - 05
--DESCRIPTION--
whitespace control - blocks - should strip whitespace around simple block calls - 05
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
$tmpl = ' a

{{~#if foo~}} 

bar 

{{~/if~}}

a ';
$context = array(
        'foo' => 'bar<',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
 abara 