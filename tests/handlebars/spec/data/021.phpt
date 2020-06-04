--TEST--
data - @root - passed root values take priority
--DESCRIPTION--
data - @root - passed root values take priority
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
        'foo' => 'should not be used',
    );
$compileOptions = array();
$runtimeOptions = array(
        'data' => array(
            'root' => array(
                'foo' => 'hello',
            ),
        ),
    );
$allOptions = array(
        'data' => array(
            'root' => array(
                'foo' => 'hello',
            ),
        ),
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
hello