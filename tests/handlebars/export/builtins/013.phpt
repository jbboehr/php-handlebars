--TEST--
builtin helpers - #if - should not change the depth list
--DESCRIPTION--
builtin helpers - #if - should not change the depth list
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
$tmpl = '{{#with foo}}{{#if goodbye}}GOODBYE cruel {{../world}}!{{/if}}{{/with}}';
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
                                    [opcode] => lookupOnContext
                                    [args] => Array
                                        (
                                            [0] => Array
                                                (
                                                    [0] => goodbye
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
                                                            [0] => GOODBYE cruel 
                                                        )

                                                )

                                            [1] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 1
                                                        )

                                                )

                                            [2] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => world
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 1
                                                        )

                                                )

                                            [3] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => resolvePossibleLambda
                                                    [args] => Array
                                                        (
                                                        )

                                                )

                                            [4] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendEscaped
                                                    [args] => Array
                                                        (
                                                        )

                                                )

                                            [5] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => !
                                                        )

                                                )

                                        )

                                    [children] => Array
                                        (
                                        )

                                    [decorators] => 
                                    [isSimple] => 
                                    [useDepths] => 1
                                    [usePartial] => 
                                    [useDecorators] => 
                                    [blockParams] => 0
                                    [stringParams] => 
                                    [trackIds] => 
                                )

                        )

                    [decorators] => 
                    [isSimple] => 
                    [useDepths] => 1
                    [usePartial] => 
                    [useDecorators] => 
                    [blockParams] => 0
                    [stringParams] => 
                    [trackIds] => 
                )

        )

    [decorators] => 
    [isSimple] => 
    [useDepths] => 1
    [usePartial] => 
    [useDecorators] => 
    [blockParams] => 0
    [stringParams] => 
    [trackIds] => 
)
string