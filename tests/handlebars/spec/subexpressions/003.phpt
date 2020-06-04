--TEST--
subexpressions - mixed paths and helpers
--DESCRIPTION--
subexpressions - mixed paths and helpers
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
$tmpl = '{{blog baz.bat (equal a b) baz.bar}}';
$context = array(
        'bar' => 'LOL',
        'baz' => array(
            'bat' => 'foo!',
            'bar' => 'bar!',
        ),
    );
$helpers = new DefaultRegistry(array(
        'blog' => function($val, $that, $theOther) {
        return "val is " . $val . ', ' . (is_bool($that) ? ($that ? 'true' : 'false') : $that) . ' and ' . $theOther;
      }/*function (val, that, theOther) {
        return 'val is ' + val + ', ' + that + ' and ' + theOther;
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
val is foo!, true and bar!