--TEST--
blocks - works with cached blocks
--DESCRIPTION--
blocks - works with cached blocks
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
$tmpl = '{{#each person}}{{#with .}}{{first}} {{last}}{{/with}}{{/each}}';
$context = array(
        'person' => array(
            array(
                'first' => 'Alan',
                'last' => 'Johnson',
            ),
            array(
                'first' => 'Alan',
                'last' => 'Johnson',
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
Alan JohnsonAlan Johnson