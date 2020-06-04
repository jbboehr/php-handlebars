--TEST--
blocks - array without data
--DESCRIPTION--
blocks - array without data
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
$tmpl = '{{#goodbyes}}{{text}}{{/goodbyes}} {{#goodbyes}}{{text}}{{/goodbyes}}';
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
goodbyeGoodbyeGOODBYE goodbyeGoodbyeGOODBYE