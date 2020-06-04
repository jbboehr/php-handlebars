--TEST--
subexpressions - provides each nested helper invocation its own options hash
--DESCRIPTION--
subexpressions - provides each nested helper invocation its own options hash
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
$tmpl = '{{equal (equal true true) true}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'equal' => function($x, $y, $options) {
           static $lastOptions = null;
        if (!$options || $options === $lastOptions) {
          throw new Exception('options hash was reused');
        }
        $lastOptions = $options;
        return $x === $y;
      }/*function(x, y, options) {
        if (!options || options === global.lastOptions) {
          throw new Error('options hash was reused');
        }
        global.lastOptions = options;
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
true