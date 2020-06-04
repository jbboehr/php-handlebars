--TEST--
Benchmarks - subexpression
--DESCRIPTION--
Benchmarks - subexpression
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
$tmpl = '{{echo (header)}}';
$context = array(
        'echo' => function($value) {
      return 'foo ' . $value;
    }/*function (value) {
      return 'foo ' + value;
    }*/,
        'header' => function() {
      return "Colors";
    }/*function () {
      return 'Colors';
    }*/,
    );
$helpers = new DefaultRegistry(array(
        false,
    ));
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array();
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
foo Colors