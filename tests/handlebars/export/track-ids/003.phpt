--TEST--
track ids - should include hash ids
--DESCRIPTION--
track ids - should include hash ids
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
  'trackIds' => true,
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
$tmpl = '{{wycats bat=is.a baz=slave.driver}}';
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
                            [0] => 
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
                    [opcode] => pushId
                    [args] => Array
                        (
                            [0] => PathExpression
                            [1] => is.a
                            [2] => 
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
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => is
                                    [1] => a
                                )

                            [1] => 
                            [2] => 
                            [3] => 
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => pushId
                    [args] => Array
                        (
                            [0] => PathExpression
                            [1] => slave.driver
                            [2] => 
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
                                    [0] => slave
                                    [1] => driver
                                )

                            [1] => 
                            [2] => 
                            [3] => 
                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => baz
                        )

                )

            [10] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => bat
                        )

                )

            [11] => Handlebars\Opcode Object
                (
                    [opcode] => popHash
                    [args] => Array
                        (
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
                                    [0] => wycats
                                )

                            [1] => 1
                            [2] => 1
                            [3] => 
                        )

                )

            [14] => Handlebars\Opcode Object
                (
                    [opcode] => invokeHelper
                    [args] => Array
                        (
                            [0] => 0
                            [1] => wycats
                            [2] => 1
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
    [usePartial] => 
    [useDecorators] => 
    [blockParams] => 0
    [stringParams] => 
    [trackIds] => 1
)
string