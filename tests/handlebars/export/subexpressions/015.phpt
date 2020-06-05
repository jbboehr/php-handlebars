--TEST--
subexpressions - in string params mode,
--DESCRIPTION--
subexpressions - in string params mode,
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
  'stringParams' => true,
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
$tmpl = '{{snog (blorg foo x=y) yeah a=b}}';
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
                    [opcode] => pushStringParam
                    [args] => Array
                        (
                            [0] => 
                            [1] => SubExpression
                        )

                )

            [2] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [3] => Handlebars\Opcode Object
                (
                    [opcode] => pushStringParam
                    [args] => Array
                        (
                            [0] => foo
                            [1] => PathExpression
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
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => pushHash
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
                    [opcode] => pushStringParam
                    [args] => Array
                        (
                            [0] => y
                            [1] => PathExpression
                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => x
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
                                    [0] => blorg
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
                            [1] => blorg
                            [2] => 1
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
                    [opcode] => pushStringParam
                    [args] => Array
                        (
                            [0] => yeah
                            [1] => PathExpression
                        )

                )

            [16] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
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
                    [opcode] => pushHash
                    [args] => Array
                        (
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
                    [opcode] => pushStringParam
                    [args] => Array
                        (
                            [0] => b
                            [1] => PathExpression
                        )

                )

            [21] => Handlebars\Opcode Object
                (
                    [opcode] => assignToHash
                    [args] => Array
                        (
                            [0] => a
                        )

                )

            [22] => Handlebars\Opcode Object
                (
                    [opcode] => popHash
                    [args] => Array
                        (
                        )

                )

            [23] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [24] => Handlebars\Opcode Object
                (
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => snog
                                )

                            [1] => 1
                            [2] => 1
                            [3] => 
                        )

                )

            [25] => Handlebars\Opcode Object
                (
                    [opcode] => invokeHelper
                    [args] => Array
                        (
                            [0] => 2
                            [1] => snog
                            [2] => 1
                        )

                )

            [26] => Handlebars\Opcode Object
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
    [stringParams] => 1
    [trackIds] => 
)
string