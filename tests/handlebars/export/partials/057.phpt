--TEST--
partials - compat mode - partials can access parents without data
--DESCRIPTION--
partials - compat mode - partials can access parents without data
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
  'data' => false,
  'compat' => true,
  'useDepths' => true,
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
$tmpl = 'Dudes: {{#dudes}}{{> dude}}{{/dudes}}';
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
                                    [0] => dudes
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
                            [0] => dudes
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
                                            [1] => dude
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
string