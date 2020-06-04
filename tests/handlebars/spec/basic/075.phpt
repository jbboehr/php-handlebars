--TEST--
basic context - this keyword in helpers - 01
--DESCRIPTION--
basic context - this keyword in helpers - 01
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
$tmpl = '{{#hellos}}{{foo this/text}}{{/hellos}}';
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
$helpers = new DefaultRegistry(array(
        'foo' => function($value) { return 'bar ' . $value; }/*function (value) {
        return 'bar ' + value;
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
bar hellobar Hellobar HELLO