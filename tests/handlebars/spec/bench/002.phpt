--TEST--
Benchmarks - array-each
--DESCRIPTION--
Benchmarks - array-each
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
$tmpl = '{{#each names}}{{name}}{{/each}}';
$context = array(
        'names' => array(
            array(
                'name' => 'Moe',
            ),
            array(
                'name' => 'Larry',
            ),
            array(
                'name' => 'Curly',
            ),
            array(
                'name' => 'Shemp',
            ),
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
MoeLarryCurlyShemp