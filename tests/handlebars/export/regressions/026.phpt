--TEST--
Regressions - GH-1135 : Context handling within each iteration
--DESCRIPTION--
Regressions - GH-1135 : Context handling within each iteration
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
$tmpl = '{{#each array}}
 1. IF: {{#if true}}{{../name}}-{{../../name}}-{{../../../name}}{{/if}}
 2. MYIF: {{#myif true}}{{../name}}={{../../name}}={{../../../name}}{{/myif}}
{{/each}}';
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
                                    [0] => array
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
                            [1] => each
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
                                            [0] =>  1. IF: 
                                        )

                                )

                            [1] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushLiteral
                                    [args] => Array
                                        (
                                            [0] => 1
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

                            [7] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] => 
 2. MYIF: 
                                        )

                                )

                            [8] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushLiteral
                                    [args] => Array
                                        (
                                            [0] => 1
                                        )

                                )

                            [9] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 1
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
                                    [opcode] => emptyHash
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
                                                    [0] => myif
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
                                            [0] => 1
                                            [1] => myif
                                            [2] => 1
                                        )

                                )

                            [15] => Handlebars\Opcode Object
                                (
                                    [opcode] => append
                                    [args] => Array
                                        (
                                        )

                                )

                            [16] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] => 

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
                                                            [0] => 1
                                                        )

                                                )

                                            [1] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => name
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 1
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

                                            [4] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => -
                                                        )

                                                )

                                            [5] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 2
                                                        )

                                                )

                                            [6] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => name
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 1
                                                        )

                                                )

                                            [7] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => resolvePossibleLambda
                                                    [args] => Array
                                                        (
                                                        )

                                                )

                                            [8] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendEscaped
                                                    [args] => Array
                                                        (
                                                        )

                                                )

                                            [9] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => -
                                                        )

                                                )

                                            [10] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 3
                                                        )

                                                )

                                            [11] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => name
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 1
                                                        )

                                                )

                                            [12] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => resolvePossibleLambda
                                                    [args] => Array
                                                        (
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

                            [1] => Handlebars\Program Object
                                (
                                    [opcodes] => Array
                                        (
                                            [0] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 1
                                                        )

                                                )

                                            [1] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => name
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 1
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

                                            [4] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => =
                                                        )

                                                )

                                            [5] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 2
                                                        )

                                                )

                                            [6] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => name
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 1
                                                        )

                                                )

                                            [7] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => resolvePossibleLambda
                                                    [args] => Array
                                                        (
                                                        )

                                                )

                                            [8] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendEscaped
                                                    [args] => Array
                                                        (
                                                        )

                                                )

                                            [9] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => =
                                                        )

                                                )

                                            [10] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => getContext
                                                    [args] => Array
                                                        (
                                                            [0] => 3
                                                        )

                                                )

                                            [11] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => name
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 1
                                                        )

                                                )

                                            [12] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => resolvePossibleLambda
                                                    [args] => Array
                                                        (
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