--TEST--
partials - inline partials - should override partials down the entire stack
--DESCRIPTION--
partials - inline partials - should override partials down the entire stack
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
$tmpl = '{{#with .}}{{#*inline "myPartial"}}success{{/inline}}{{#with .}}{{#with .}}{{> myPartial}}{{/with}}{{/with}}{{/with}}';
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
                    [opcode] => pushContext
                    [args] => Array
                        (
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
                            [0] => 
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
                            [1] => with
                        )

                )

            [6] => Handlebars\Opcode Object
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
                                    [opcode] => pushString
                                    [args] => Array
                                        (
                                            [0] => myPartial
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
                                    [opcode] => emptyHash
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [4] => Handlebars\Opcode Object
                                (
                                    [opcode] => registerDecorator
                                    [args] => Array
                                        (
                                            [0] => 1
                                            [1] => inline
                                        )

                                )

                            [5] => Handlebars\Opcode Object
                                (
                                    [opcode] => getContext
                                    [args] => Array
                                        (
                                            [0] => 0
                                        )

                                )

                            [6] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushContext
                                    [args] => Array
                                        (
                                        )

                                )

                            [7] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 1
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
                                    [opcode] => emptyHash
                                    [args] => Array
                                        (
                                            [0] => 
                                        )

                                )

                            [10] => Handlebars\Opcode Object
                                (
                                    [opcode] => invokeKnownHelper
                                    [args] => Array
                                        (
                                            [0] => 1
                                            [1] => with
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
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => success
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
                                                    [opcode] => pushContext
                                                    [args] => Array
                                                        (
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
                                                            [0] => 
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
                                                            [1] => with
                                                        )

                                                )

                                            [6] => Handlebars\Opcode Object
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
                                                                    [opcode] => pushContext
                                                                    [args] => Array
                                                                        (
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
                                                                    [opcode] => pushProgram
                                                                    [args] => Array
                                                                        (
                                                                            [0] => 
                                                                        )

                                                                )

                                                            [4] => Handlebars\Opcode Object
                                                                (
                                                                    [opcode] => emptyHash
                                                                    [args] => Array
                                                                        (
                                                                            [0] => 1
                                                                        )

                                                                )

                                                            [5] => Handlebars\Opcode Object
                                                                (
                                                                    [opcode] => invokePartial
                                                                    [args] => Array
                                                                        (
                                                                            [0] => 
                                                                            [1] => myPartial
                                                                            [2] => 
                                                                        )

                                                                )

                                                            [6] => Handlebars\Opcode Object
                                                                (
                                                                    [opcode] => append
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
                                                    [usePartial] => 1
                                                    [useDecorators] => 
                                                    [blockParams] => 0
                                                    [stringParams] => 
                                                    [trackIds] => 
                                                )

                                        )

                                    [decorators] => 
                                    [isSimple] => 
                                    [useDepths] => 
                                    [usePartial] => 1
                                    [useDecorators] => 
                                    [blockParams] => 0
                                    [stringParams] => 
                                    [trackIds] => 
                                )

                        )

                    [decorators] => 
                    [isSimple] => 
                    [useDepths] => 
                    [usePartial] => 1
                    [useDecorators] => 1
                    [blockParams] => 0
                    [stringParams] => 
                    [trackIds] => 
                )

        )

    [decorators] => 
    [isSimple] => 
    [useDepths] => 
    [usePartial] => 1
    [useDecorators] => 
    [blockParams] => 0
    [stringParams] => 
    [trackIds] => 
)
string