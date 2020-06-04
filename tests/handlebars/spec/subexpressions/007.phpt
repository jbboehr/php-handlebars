--TEST--
subexpressions - GH-800 : Complex subexpressions - 02
--DESCRIPTION--
subexpressions - GH-800 : Complex subexpressions - 02
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
$tmpl = '{{dash c.c (concat a b)}}';
$context = array(
        'a' => 'a',
        'b' => 'b',
        'c' => array(
            'c' => 'c',
        ),
        'd' => 'd',
        'e' => array(
            'e' => 'e',
        ),
    );
$helpers = new DefaultRegistry(array(
        'dash' => function($a, $b) {
        return $a . "-" . $b;
      }/*function (a, b) {
        return a + '-' + b;
      }*/,
        'concat' => function($a, $b) {
        return $a . $b;
      }/*function (a, b) {
        return a + b;
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
c-ab