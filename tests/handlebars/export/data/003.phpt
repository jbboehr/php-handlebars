--TEST--
data - deep @foo triggers automatic top-level data
--DESCRIPTION--
data - deep @foo triggers automatic top-level data
Automatic data was triggered
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
);
$tmpl = '{{#let world="world"}}{{#if foo}}{{#if foo}}Hello {{@world}}{{/if}}{{/if}}{{/let}}';
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
                    [opcode] => pushString
                    [args] => Array
                        (
                            [0] => world
                        )

                )

            [4] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => world
                        )

                )

            [5] => Handlebars\Opcode Object
                (
                    [opcode] => popHash
                    [args] => Array
                        (
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [7] => Handlebars\Opcode Object
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

            [8] => Handlebars\Opcode Object
                (
                    [opcode] => invokeHelper
                    [args] => Array
                        (
                            [0] => 0
                            [1] => let
                            [2] => 1
                        )

                )

            [9] => Handlebars\Opcode Object
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
                                    [opcode] => lookupOnContext
                                    [args] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => foo
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
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => foo
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
                                                                            [0] => Hello 
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
                                                                    [opcode] => getContext
                                                                    [args] => Array
                                                                        (
                                                                            [0] => 0
                                                                        )

                                                                )

                                                            [5] => Handlebars\Opcode Object
                                                                (
                                                                    [opcode] => lookupData
                                                                    [args] => Array
                                                                        (
                                                                            [0] => 0
                                                                            [1] => Array
                                                                                (
                                                                                    [0] => world
                                                                                )

                                                                            [2] => 1
                                                                        )

                                                                )

                                                            [6] => Handlebars\Opcode Object
                                                                (
                                                                    [opcode] => invokeAmbiguous
                                                                    [args] => Array
                                                                        (
                                                                            [0] => world
                                                                            [1] => 
                                                                        )

                                                                )

                                                            [7] => Handlebars\Opcode Object
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