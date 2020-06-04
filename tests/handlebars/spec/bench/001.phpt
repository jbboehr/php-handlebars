--TEST--
Benchmarks - arguments
--DESCRIPTION--
Benchmarks - arguments
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
$tmpl = '{{foo person "person" 1 true foo=bar foo="person" foo=1 foo=true}}';
$context = array(
        'bar' => true,
    );
$helpers = new DefaultRegistry(array(
        'foo' => function() {
      return '';
    }/*function () {
      return '';
    }*/,
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