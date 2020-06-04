--TEST--
basic context - this keyword in paths - 01
--DESCRIPTION--
basic context - this keyword in paths - 01
This keyword evaluates in more complex paths
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
$tmpl = '{{#hellos}}{{this/text}}{{/hellos}}';
$context = array(
        'hellos' => array(
            array(
                'text' => 'hello',
            ),
            array(
                'text' => 'Hello',
            ),
            array(
                'text' => 'HELLO',
            ),
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
helloHelloHELLO