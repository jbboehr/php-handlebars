--TEST--
blocks - empty block
--DESCRIPTION--
blocks - empty block
Arrays iterate over the contents when not empty
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
$tmpl = '{{#goodbyes}}{{/goodbyes}}cruel {{world}}!';
$context = array(
        'goodbyes' => array(
            array(
                'text' => 'goodbye',
            ),
            array(
                'text' => 'Goodbye',
            ),
            array(
                'text' => 'GOODBYE',
            ),
        ),
        'world' => 'world',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
cruel world!