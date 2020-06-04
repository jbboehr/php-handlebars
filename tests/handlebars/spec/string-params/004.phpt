--TEST--
string params mode - information about the types is passed along
--DESCRIPTION--
string params mode - information about the types is passed along
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
$tmpl = '{{tomdale \'need\' dad.joke true false}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'tomdale' => function () { throw new Exception("unimplemented test fixture"); }/*function(desire, noun, trueBool, falseBool, options) {
        equal(options.types[0], 'StringLiteral', 'the string type is passed');
        equal(
          options.types[1],
          'PathExpression',
          'the expression type is passed'
        );
        equal(
          options.types[2],
          'BooleanLiteral',
          'the expression type is passed'
        );
        equal(desire, 'need', 'the string form is passed for strings');
        equal(noun, 'dad.joke', 'the string form is passed for expressions');
        equal(trueBool, true, 'raw booleans are passed through');
        equal(falseBool, false, 'raw booleans are passed through');
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