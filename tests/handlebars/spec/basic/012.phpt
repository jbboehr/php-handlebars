--TEST--
basic context - comments - 01
--DESCRIPTION--
basic context - comments - 01
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
$tmpl = '    {{~! comment ~}}      blah';
$context = array();
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
blah