--TEST--
Regressions - bug reported by @fat where lambdas weren't being properly resolved
--DESCRIPTION--
Regressions - bug reported by @fat where lambdas weren't being properly resolved
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
$tmpl = '<strong>This is a slightly more complicated {{thing}}.</strong>.
{{! Just ignore this business. }}
Check this out:
{{#hasThings}}
<ul>
{{#things}}
<li class={{className}}>{{word}}</li>
{{/things}}</ul>.
{{/hasThings}}
{{^hasThings}}

<small>Nothing to check out...</small>
{{/hasThings}}';
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
                            [0] => <strong>This is a slightly more complicated 
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
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => thing
                                )

                            [1] => 
                            [2] => 1
                            [3] => 
                        )

                )

            [6] => Handlebars\Opcode Object
                (
                    [opcode] => invokeAmbiguous
                    [args] => Array
                        (
                            [0] => thing
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

            [8] => Handlebars\Opcode Object
                (
                    [opcode] => appendContent
                    [args] => Array
                        (
                            [0] => .</strong>.

                        )

                )

            [9] => Handlebars\Opcode Object
                (
                    [opcode] => appendContent
                    [args] => Array
                        (
                            [0] => Check this out:

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
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 0
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
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [14] => Handlebars\Opcode Object
                (
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => hasThings
                                )

                            [1] => 
                            [2] => 1
                            [3] => 
                        )

                )

            [15] => Handlebars\Opcode Object
                (
                    [opcode] => invokeAmbiguous
                    [args] => Array
                        (
                            [0] => hasThings
                            [1] => 1
                        )

                )

            [16] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
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
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [19] => Handlebars\Opcode Object
                (
                    [opcode] => ambiguousBlockValue
                    [args] => Array
                        (
                        )

                )

            [20] => Handlebars\Opcode Object
                (
                    [opcode] => append
                    [args] => Array
                        (
                        )

                )

            [21] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [22] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [23] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 1
                        )

                )

            [24] => Handlebars\Opcode Object
                (
                    [opcode] => getContext
                    [args] => Array
                        (
                            [0] => 0
                        )

                )

            [25] => Handlebars\Opcode Object
                (
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => hasThings
                                )

                            [1] => 
                            [2] => 1
                            [3] => 
                        )

                )

            [26] => Handlebars\Opcode Object
                (
                    [opcode] => invokeAmbiguous
                    [args] => Array
                        (
                            [0] => hasThings
                            [1] => 1
                        )

                )

            [27] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [28] => Handlebars\Opcode Object
                (
                    [opcode] => pushProgram
                    [args] => Array
                        (
                            [0] => 1
                        )

                )

            [29] => Handlebars\Opcode Object
                (
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [30] => Handlebars\Opcode Object
                (
                    [opcode] => ambiguousBlockValue
                    [args] => Array
                        (
                        )

                )

            [31] => Handlebars\Opcode Object
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
                                            [0] => <ul>

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
                                                    [0] => things
                                                )

                                            [1] => 
                                            [2] => 1
                                            [3] => 
                                        )

                                )

                            [6] => Handlebars\Opcode Object
                                (
                                    [opcode] => invokeAmbiguous
                                    [args] => Array
                                        (
                                            [0] => things
                                            [1] => 1
                                        )

                                )

                            [7] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 0
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
                                    [opcode] => ambiguousBlockValue
                                    [args] => Array
                                        (
                                        )

                                )

                            [11] => Handlebars\Opcode Object
                                (
                                    [opcode] => append
                                    [args] => Array
                                        (
                                        )

                                )

                            [12] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] => </ul>.

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
                                                            [0] => <li class=
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
                                                    [opcode] => lookupOnContext
                                                    [args] => Array
                                                        (
                                                            [0] => Array
                                                                (
                                                                    [0] => className
                                                                )

                                                            [1] => 
                                                            [2] => 1
                                                            [3] => 
                                                        )

                                                )

                                            [6] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => invokeAmbiguous
                                                    [args] => Array
                                                        (
                                                            [0] => className
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

                                            [8] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => >
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
                                                                    [0] => word
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
                                                            [0] => word
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

                                            [16] => Handlebars\Opcode Object
                                                (
                                                    [opcode] => appendContent
                                                    [args] => Array
                                                        (
                                                            [0] => </li>

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

            [1] => Handlebars\Program Object
                (
                    [opcodes] => Array
                        (
                            [0] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] => 
<small>Nothing to check out...</small>

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