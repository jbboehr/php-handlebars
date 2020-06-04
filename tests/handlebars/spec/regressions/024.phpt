--TEST--
Regressions - GH-1065: Sparse arrays
--DESCRIPTION--
Regressions - GH-1065: Sparse arrays
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
$tmpl = '{{#each array}}{{@index}}{{.}}{{/each}}';
$context = array(
        'array' => array(
            1 => 'foo',
            3 => 'bar',
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
1foo3bar