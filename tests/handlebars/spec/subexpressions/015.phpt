--TEST--
subexpressions - in string params mode,
--DESCRIPTION--
subexpressions - in string params mode,
--SKIPIF--
<?php
if( true ) die('skip string params are not supported by the VM');
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
$tmpl = '{{snog (blorg foo x=y) yeah a=b}}';
$context = array(
        'foo' => array(),
        'yeah' => array(),
    );
$helpers = new DefaultRegistry(array(
        'snog' => function($a, $b, $options) {
        return $a . $b;
      }/*function (a, b, options) {
        equals(a, 'foo');
        equals(options.types.length, 2, 'string params for outer helper processed correctly');
        equals(options.types[0], 'SubExpression', 'string params for outer helper processed correctly');
        equals(options.types[1], 'PathExpression', 'string params for outer helper processed correctly');
        return a + b;
      }*/,
        'blorg' => function($a, $options) {
        return $a;
      }/*function (a, options) {
        equals(options.types.length, 1, 'string params for inner helper processed correctly');
        equals(options.types[0], 'PathExpression', 'string params for inner helper processed correctly');
        return a;
      }*/,
    ));
$compileOptions = array(
        'stringParams' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'stringParams' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
fooyeah