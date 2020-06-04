--TEST--
subexpressions - helper w args
--DESCRIPTION--
subexpressions - helper w args
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
$tmpl = '{{blog (equal a b)}}';
$context = array(
        'bar' => 'LOL',
    );
$helpers = new DefaultRegistry(array(
        'blog' => function($val) {
        return "val is " . (is_bool($val) ? ($val ? 'true' : 'false') : $val);
      }/*function (val) {
        return 'val is ' + val;
      }*/,
        'equal' => function($x, $y) {
        return $x === $y;
      }/*function (x, y) {
        return x === y;
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
val is true