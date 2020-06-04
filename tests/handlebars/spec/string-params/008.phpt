--TEST--
string params mode - should handle DATA
--DESCRIPTION--
string params mode - should handle DATA
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
$tmpl = '{{foo @bar}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'foo' => function($bar, $options) {
        return 'Foo!';
      }/*function (bar, options) {
        equal(bar, '@bar');
        equal(options.types[0], 'PathExpression');
        return 'Foo!';
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
Foo!