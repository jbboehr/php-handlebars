--TEST--
partials - GH-14: a partial preceding a selector
--DESCRIPTION--
partials - GH-14: a partial preceding a selector
Regular selectors can follow a partial
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
$tmpl = 'Dudes: {{>dude}} {{anotherDude}}';
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
                            [0] => Dudes: 
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
                    [opcode] => pushContext
                    [args] => Array
                        (
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
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [5] => Handlebars\Opcode Object
                (
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 1
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => invokePartial
                    [args] => Array
                        (
                            [0] => 
                            [1] => dude
                            [2] => 
                        )

                )

            [7] => Handlebars\Opcode Object
                (
                    [opcode] => append
                    [args] => Array
                        (
                        )

                )

            [8] => Handlebars\Opcode Object
                (
                    [opcode] => appendContent
                    [args] => Array
                        (
                            [0] =>  
                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
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
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [12] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [13] => Handlebars\Opcode Object
                (
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => anotherDude
                                )

                            [1] => 
                            [2] => 1
                            [3] => 
                        )

                )

            [14] => Handlebars\Opcode Object
                (
                    [opcode] => invokeAmbiguous
                    [args] => Array
                        (
                            [0] => anotherDude
                            [1] => 
                        )

                )

            [15] => Handlebars\Opcode Object
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
    [usePartial] => 1
    [useDecorators] => 
    [blockParams] => 0
    [stringParams] => 
    [trackIds] => 
)
string