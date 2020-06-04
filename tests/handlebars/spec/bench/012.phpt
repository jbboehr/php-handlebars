--TEST--
Benchmarks - paths
--DESCRIPTION--
Benchmarks - paths
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
$tmpl = '{{person.name.bar.baz}}{{person.age}}{{person.foo}}{{animal.age}}';
$context = array(
        'person' => array(
            'name' => array(
                'bar' => array(
                    'baz' => 'Larry',
                ),
            ),
            'age' => 45,
        ),
    );
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array();
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Larry45