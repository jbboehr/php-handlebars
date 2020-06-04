--TEST--
string params mode - hash parameters get type information
--DESCRIPTION--
string params mode - hash parameters get type information
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
$tmpl = '{{tomdale he.says desire=\'need\' noun=dad.joke bool=true}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'tomdale' => function () { throw new Exception("unimplemented test fixture"); }/*function(exclamation, options) {
        equal(exclamation, 'he.says');
        equal(options.types[0], 'PathExpression');

        equal(options.hashTypes.desire, 'StringLiteral');
        equal(options.hashTypes.noun, 'PathExpression');
        equal(options.hashTypes.bool, 'BooleanLiteral');
        equal(options.hash.desire, 'need');
        equal(options.hash.noun, 'dad.joke');
        equal(options.hash.bool, true);
        return 'Helper called';
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
Helper called