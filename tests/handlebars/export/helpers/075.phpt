--TEST--
helpers - block params - should allow block params on chained helpers
--DESCRIPTION--
helpers - block params - should allow block params on chained helpers
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
);
$tmpl = '{{#if bar}}{{else goodbyes as |value|}}{{value}}{{/if}}{{value}}';
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
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => bar
                                )

                            [1] => 
                            [2] => 
                            [3] => 
                        )

                )

            [2] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [3] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 1
                        )

                )

            [4] => Handlebars\Opcode Object
                (
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [5] => Handlebars\Opcode Object
                (
                    [opcode] => invokeKnownHelper
                    [args] => Array
                        (
                            [0] => 1
                            [1] => if
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => append
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
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => value
                                )

                            [1] => 
                            [2] => 1
                            [3] => 
                        )

                )

            [12] => Handlebars\Opcode Object
                (
                    [opcode] => invokeAmbiguous
                    [args] => Array
                        (
                            [0] => value
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
            [0] => Handlebars\Program Object
                (
                    [opcodes] => Array
                        (
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

            [1] => Handlebars\Program Object
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
                                            [0] => 0
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
                                    [opcode] => lookupOnContext
                                    [args] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => goodbyes
                                                )

                                            [1] => 
                                            [2] => 1
                                            [3] => 
                                        )

                                )

                            [5] => Handlebars\Opcode Object
                                (
                                    [opcode] => invokeAmbiguous
                                    [args] => Array
                                        (
                                            [0] => goodbyes
                                            [1] => 1
                                        )

                                )

                            [6] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [7] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [8] => Handlebars\Opcode Object
                                (
                                    [opcode] => emptyHash
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [9] => Handlebars\Opcode Object
                                (
                                    [opcode] => ambiguousBlockValue
                                    [args] => Array
                                        (
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
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 0
                                                        )

                                                )

                                            [1] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupBlockParam
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => 0
                                                                    [1] => 0
                                                                )

                                                            [1] => Array
                                                                (
                                                                    [0] => value
                                                                )

                                                        )

                                                )

                                            [2] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => resolvePossibleLambda
                                                    [args] => Array
                                                        (
                                                        )

                                                )

                                            [3] => Handlebars\Opcode Object
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
                                    [blockParams] => 1
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