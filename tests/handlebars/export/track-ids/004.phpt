--TEST--
track ids - should note ../ and ./ references
--DESCRIPTION--
track ids - should note ../ and ./ references
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
$tmpl = '{{wycats ./is.a ../slave.driver this.is.a this}}';
myprint(Compiler::compile($tmpl, $compileOptions), true);
myprint(gettype(Compiler::compilePrint($tmpl, $compileOptions)), true);
--EXPECT--
Handlebars\Program Object
(
    [opcodes] => Array
        (
            [0] => Handlebars\Opcode Object
                (
                    [opcode] => pushId
                    [args] => Array
                        (
                            [0] => PathExpression
                            [1] => is.a
                            [2] => 
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
                                    [0] => is
                                    [1] => a
                                )

                            [1] => 
                            [2] => 
                            [3] => 1
                        )

                )

            [3] => Handlebars\Opcode Object
                (
                    [opcode] => pushId
                    [args] => Array
                        (
                            [0] => PathExpression
                            [1] => ../slave.driver
                            [2] => 
                        )

                )

            [4] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 1
                        )

                )

            [5] => Handlebars\Opcode Object
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
                            [3] => 1
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => pushId
                    [args] => Array
                        (
                            [0] => PathExpression
                            [1] => is.a
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
                                    [0] => is
                                    [1] => a
                                )

                            [1] => 
                            [2] => 
                            [3] => 1
                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => pushId
                    [args] => Array
                        (
                            [0] => PathExpression
                            [1] => 
                            [2] => 
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
                    [opcode] => pushContext
                    [args] => Array
                        (
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
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [15] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [16] => Handlebars\Opcode Object
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

            [17] => Handlebars\Opcode Object
                (
                    [opcode] => invokeHelper
                    [args] => Array
                        (
                            [0] => 4
                            [1] => wycats
                            [2] => 1
                        )

                )

            [18] => Handlebars\Opcode Object
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
    [useDepths] => 1
    [usePartial] => 
    [useDecorators] => 
    [blockParams] => 0
    [stringParams] => 
    [trackIds] => 1
)
string