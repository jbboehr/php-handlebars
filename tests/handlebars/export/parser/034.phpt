--TEST--
parser - parses mustaches with hash arguments - 08
--DESCRIPTION--
parser - parses mustaches with hash arguments - 08
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
$tmpl = '{{foo omg bar=baz bat="bam"}}';
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
                                    [0] => omg
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
                    [opcode] => pushHash
                    [args] => Array
                        (
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
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => baz
                                )

                            [1] => 
                            [2] => 
                            [3] => 
                        )

                )

            [7] => Handlebars\Opcode Object
                (
                    [opcode] => pushString
                    [args] => Array
                        (
                            [0] => bam
                        )

                )

            [8] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => bat
                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => bar
                        )

                )

            [10] => Handlebars\Opcode Object
                (
                    [opcode] => popHash
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
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => foo
                                )

                            [1] => 1
                            [2] => 1
                            [3] => 
                        )

                )

            [13] => Handlebars\Opcode Object
                (
                    [opcode] => invokeHelper
                    [args] => Array
                        (
                            [0] => 1
                            [1] => foo
                            [2] => 1
                        )

                )

            [14] => Handlebars\Opcode Object
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
string