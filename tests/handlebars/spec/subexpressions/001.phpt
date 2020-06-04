--TEST--
subexpressions - arg-less helper
--DESCRIPTION--
subexpressions - arg-less helper
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
$tmpl = '{{foo (bar)}}!';
$context = array();
$helpers = new DefaultRegistry(array(
        'foo' => function($val) {
        return $val . $val;
      }/*function (val) {
        return val + val;
      }*/,
        'bar' => function() {
        return "LOL";
      }/*function () {
        return 'LOL';
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
LOLLOL!