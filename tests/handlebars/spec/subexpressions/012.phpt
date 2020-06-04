--TEST--
subexpressions - as hashes
--DESCRIPTION--
subexpressions - as hashes
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
$tmpl = '{{blog fun=(equal (blog fun=1) \'val is 1\')}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'blog' => function($options) {
        return "val is " . (is_bool($tmp = $options['hash']['fun']) ? ($tmp ? 'true' : 'false') : $tmp);
      }/*function (options) {
        return "val is " + options.hash.fun;
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