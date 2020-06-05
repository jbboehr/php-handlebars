--TEST--
string params mode - when using block form, arguments to helpers can be retrieved from options hash in string form
--DESCRIPTION--
string params mode - when using block form, arguments to helpers can be retrieved from options hash in string form
String parameters output
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( !class_exists('Handlebars\Compiler') ) die('skip handlebars AST not enabled');
?>
--FILE--
<?php
use Handlebars\Compiler;
use Handlebars\Parser;
use Handlebars\Tokenizer;
use Handlebars\Utils;
use Handlebars\VM;
require __DIR__ . "/../../../utils.inc";
$compileOptions = array (
  'stringParams' => true,
  'data' => true,
  'blockParams' => 
  array (
  ),
  'knownHelpers' => 
  array (
    'helperMissing' => true,
    'blockHelperMissing' => true,
    'each' => true,
    'if' => true,
    'unless' => true,
    'with' => true,
    'log' => true,
    'lookup' => true,
  ),
);
$tmpl = '{{#wycats is.a slave.driver}}help :({{/wycats}}';
myprint(Compiler::compile($tmpl, $compileOptions), true);
myprint(gettype(Compiler::compilePrint($tmpl, $compileOptions)), true);
--EXPECT--
Handlebars\Program Object
(
    [opcodes] => Array
        (
            [0] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [1] => Handlebars\Opcode Object
                (
                    [opcode] => pushStringParam
                    [args] => Array
                        (
                            [0] => is.a
                            [1] => PathExpression
                        )

                )

            [2] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [3] => Handlebars\Opcode Object
                (
                    [opcode] => pushStringParam
                    [args] => Array
                        (
                            [0] => slave.driver
                            [1] => PathExpression
                        )

                )

            [4] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [5] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [7] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [8] => Handlebars\Opcode Object
                (
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => wycats
                                )

                            [1] => 1
                            [2] => 1
                            [3] => 
                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => invokeHelper
                    [args] => Array
                        (
                            [0] => 2
                            [1] => wycats
                            [2] => 1
                        )

                )

            [10] => Handlebars\Opcode Object
                (
                    [opcode] => append
                    [args] => Array
                        (
                        )

                )

        )

    [children] => Array
        (
            [0] => Handlebars\Program Object
                (
                    [opcodes] => Array
                        (
                            [0] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] => help :(
                                        )

                                )

                        )

                    [children] => Array
                        (
                        )

                    [decorators] => 
                    [isSimple] => 
                    [useDepths] => 
                    [usePartial] => 
                    [useDecorators] => 
                    [blockParams] => 0
                    [stringParams] => 1
                    [trackIds] => 
                )

        )

    [decorators] => 
    [isSimple] => 
    [useDepths] => 
    [usePartial] => 
    [useDecorators] => 
    [blockParams] => 0
    [stringParams] => 1
    [trackIds] => 
)
string