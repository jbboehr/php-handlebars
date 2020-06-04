--TEST--
data - @root - the root context can be looked up via @root
--DESCRIPTION--
data - @root - the root context can be looked up via @root
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
$tmpl = '{{@root.foo}}';
$context = array(
        'foo' => 'hello',
    );
$compileOptions = array();
$runtimeOptions = array(
        'data' => array(),
    );
$allOptions = array(
        'data' => array(),
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
hello