--TEST--
helpers - multiple parameters - block multi-params work
--DESCRIPTION--
helpers - multiple parameters - block multi-params work
block helpers with multiple params
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
$tmpl = 'Message: {{#goodbye cruel world}}{{greeting}} {{adj}} {{noun}}{{/goodbye}}';
myprint(Compiler::compile($tmpl, $compileOptions), true);
myprint(gettype(Compiler::compilePrint($tmpl, $compileOptions)), true);
--EXPECT--
Handlebars\Program Object
(
    [opcodes] => Array
        (
            [0] => Handlebars\Opcode Object
                (
                    [opcode] => appendContent
                    [args] => Array
                        (
                            [0] => Message: 
                        )

                )

            [1] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [2] => Handlebars\Opcode Object
                (
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => cruel
                                )

                            [1] => 
                            [2] => 
                            [3] => 
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
                                    [0] => world
                                )

                            [1] => 
                            [2] => 
                            [3] => 
                        )

                )

            [5] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [7] => Handlebars\Opcode Object
                (
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 
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
                                    [0] => goodbye
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
                            [0] => 2
                            [1] => goodbye
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
                                    [opcode] => lookupOnContext
                                    [args] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => greeting
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
                                            [0] => greeting
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
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] =>  
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
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [10] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
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
                                    [opcode] => lookupOnContext
                                    [args] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => adj
                                                )

                                            [1] => 
                                            [2] => 1
                                            [3] => 
                                        )

                                )

                            [13] => Handlebars\Opcode Object
                                (
                                    [opcode] => invokeAmbiguous
                                    [args] => Array
                                        (
                                            [0] => adj
                                            [1] => 
                                        )

                                )

                            [14] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendEscaped
                                    [args] => Array
                                        (
                                        )

                                )

                            [15] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] =>  
                                        )

                                )

                            [16] => Handlebars\Opcode Object
                                (
                                    [opcode] => getContext
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [17] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [18] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [19] => Handlebars\Opcode Object
                                (
                                    [opcode] => getContext
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [20] => Handlebars\Opcode Object
                                (
                                    [opcode] => lookupOnContext
                                    [args] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => noun
                                                )

                                            [1] => 
                                            [2] => 1
                                            [3] => 
                                        )

                                )

                            [21] => Handlebars\Opcode Object
                                (
                                    [opcode] => invokeAmbiguous
                                    [args] => Array
                                        (
                                            [0] => noun
                                            [1] => 
                                        )

                                )

                            [22] => Handlebars\Opcode Object
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
string