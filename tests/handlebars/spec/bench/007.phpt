--TEST--
Benchmarks - depth-2
--DESCRIPTION--
Benchmarks - depth-2
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
$tmpl = '{{#each names}}{{#each name}}{{../bat}}{{../../foo}}{{/each}}{{/each}}';
$context = array(
        'names' => array(
            array(
                'bat' => 'foo',
                'name' => array(
                    'Moe',
                ),
            ),
            array(
                'bat' => 'foo',
                'name' => array(
                    'Larry',
                ),
            ),
            array(
                'bat' => 'foo',
                'name' => array(
                    'Curly',
                ),
            ),
            array(
                'bat' => 'foo',
                'name' => array(
                    'Shemp',
                ),
            ),
        ),
        'foo' => 'bar',
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
foobarfoobarfoobarfoobar