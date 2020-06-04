--TEST--
Benchmarks - partial-recursion
--DESCRIPTION--
Benchmarks - partial-recursion
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
$tmpl = '{{name}}{{#each kids}}{{>recursion}}{{/each}}';
$context = array(
        'name' => '1',
        'kids' => array(
            array(
                'name' => '1.1',
                'kids' => array(
                    array(
                        'name' => '1.1.1',
                        'kids' => array(),
                    ),
                ),
            ),
        ),
    );
$partials = new DefaultRegistry(array(
        'recursion' => '{{name}}{{#each kids}}{{>recursion}}{{/each}}',
    ));
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array();
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
11.11.1.1