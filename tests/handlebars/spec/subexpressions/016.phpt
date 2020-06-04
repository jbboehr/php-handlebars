--TEST--
subexpressions - as hashes in string params mode
--DESCRIPTION--
subexpressions - as hashes in string params mode
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
$tmpl = '{{blog fun=(bork)}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'blog' => function () { throw new Exception("unimplemented test fixture"); }/*function(options) {
        equals(options.hashTypes.fun, 'SubExpression');
        return 'val is ' + options.hash.fun;
      }*/,
        'bork' => function() {
        return "BORK";
      }/*function () {
        return 'BORK';
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
val is BORK