--TEST--
data - data is inherited downstream
--DESCRIPTION--
data - data is inherited downstream
data variables are inherited downstream
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
$compileOptions = array (
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
$tmpl = '{{#let foo=1 bar=2}}{{#let foo=bar.baz}}{{@bar}}{{@foo}}{{/let}}{{@foo}}{{/let}}';
myprint(Compiler::compile($tmpl, $compileOptions), true);
myprint(gettype(Compiler::compilePrint($tmpl, $compileOptions)), true);
--EXPECT--
Handlebars\Program Object
(
    [opcodes] => Array
        (
            [0] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [1] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [2] => Handlebars\Opcode Object
                (
                    [opcode] => pushHash
                    [args] => Array
                        (
                        )

                )

            [3] => Handlebars\Opcode Object
                (
                    [opcode] => pushLiteral
                    [args] => Array
                        (
                            [0] => 1
                        )

                )

            [4] => Handlebars\Opcode Object
                (
                    [opcode] => pushLiteral
                    [args] => Array
                        (
                            [0] => 2
                        )

                )

            [5] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => bar
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => foo
                        )

                )

            [7] => Handlebars\Opcode Object
                (
                    [opcode] => popHash
                    [args] => Array
                        (
                        )

                )

            [8] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => let
                                )

                            [1] => 1
                            [2] => 1
                            [3] => 
                        )

                )

            [10] => Handlebars\Opcode Object
                (
                    [opcode] => invokeHelper
                    [args] => Array
                        (
                            [0] => 0
                            [1] => let
                            [2] => 1
                        )

                )

            [11] => Handlebars\Opcode Object
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
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [1] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [2] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushHash
                                    [args] => Array
                                        (
                                        )

                                )

                            [3] => Handlebars\Opcode Object
                                (
                                    [opcode] => getContext
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [4] => Handlebars\Opcode Object
                                (
                                    [opcode] => lookupOnContext
                                    [args] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => bar
                                                    [1] => baz
                                                )

                                            [1] => 
                                            [2] => 
                                            [3] => 
                                        )

                                )

                            [5] => Handlebars\Opcode Object
                                (
                                    [opcode] => assignToHash
                                    [args] => Array
                                        (
                                            [0] => foo
                                        )

                                )

                            [6] => Handlebars\Opcode Object
                                (
                                    [opcode] => popHash
                                    [args] => Array
                                        (
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
                                                    [0] => let
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
                                            [0] => 0
                                            [1] => let
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

                            [11] => Handlebars\Opcode Object
                                (
                                    [opcode] => getContext
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [12] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [13] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [14] => Handlebars\Opcode Object
                                (
                                    [opcode] => getContext
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [15] => Handlebars\Opcode Object
                                (
                                    [opcode] => lookupData
                                    [args] => Array
                                        (
                                            [0] => 0
                                            [1] => Array
                                                (
                                                    [0] => foo
                                                )

                                            [2] => 1
                                        )

                                )

                            [16] => Handlebars\Opcode Object
                                (
                                    [opcode] => invokeAmbiguous
                                    [args] => Array
                                        (
                                            [0] => foo
                                            [1] => 
                                        )

                                )

                            [17] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendEscaped
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
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 0
                                                        )

                                                )

                                            [1] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => pushProgram
                                                    [args] => Array
                                                        (
                                                            [0] => 
                                                        )

                                                )

                                            [2] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => pushProgram
                                                    [args] => Array
                                                        (
                                                            [0] => 
                                                        )

                                                )

                                            [3] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 0
                                                        )

                                                )

                                            [4] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupData
                                                    [args] => Array
                                                        (
                                                            [0] => 0
                                                            [1] => Array
                                                                (
                                                                    [0] => bar
                                                                )

                                                            [2] => 1
                                                        )

                                                )

                                            [5] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => invokeAmbiguous
                                                    [args] => Array
                                                        (
                                                            [0] => bar
                                                            [1] => 
                                                        )

                                                )

                                            [6] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendEscaped
                                                    [args] => Array
                                                        (
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
                                                    [opcode] => pushProgram
                                                    [args] => Array
                                                        (
                                                            [0] => 
                                                        )

                                                )

                                            [9] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => pushProgram
                                                    [args] => Array
                                                        (
                                                            [0] => 
                                                        )

                                                )

                                            [10] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 0
                                                        )

                                                )

                                            [11] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupData
                                                    [args] => Array
                                                        (
                                                            [0] => 0
                                                            [1] => Array
                                                                (
                                                                    [0] => foo
                                                                )

                                                            [2] => 1
                                                        )

                                                )

                                            [12] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => invokeAmbiguous
                                                    [args] => Array
                                                        (
                                                            [0] => foo
                                                            [1] => 
                                                        )

                                                )

                                            [13] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendEscaped
                                                    [args] => Array
                                                        (
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
                                    [stringParams] => 
                                    [trackIds] => 
                                )

                        )

                    [decorators] => 
                    [isSimple] => 
                    [useDepths] => 
                    [usePartial] => 
                    [useDecorators] => 
                    [blockParams] => 0
                    [stringParams] => 
                    [trackIds] => 
                )

        )

    [decorators] => 
    [isSimple] => 
    [useDepths] => 
    [usePartial] => 
    [useDecorators] => 
    [blockParams] => 0
    [stringParams] => 
    [trackIds] => 
)
string