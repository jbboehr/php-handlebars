--TEST--
Benchmarks - complex
--DESCRIPTION--
Benchmarks - complex
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
$tmpl = '<h1>{{header}}</h1>
{{#if items}}
  <ul>
    {{#each items}}
      {{#if current}}
        <li><strong>{{name}}</strong></li>
      {{^}}
        <li><a href="{{url}}">{{name}}</a></li>
      {{/if}}
    {{/each}}
  </ul>
{{^}}
  <p>The list is empty.</p>
{{/if}}
';
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
                            [0] => <h1>
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
                                    [0] => header
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
                            [0] => header
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
                            [0] => </h1>

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
                    [opcode] => lookupOnContext
                    [args] => Array
                        (
                            [0] => Array
                                (
                                    [0] => items
                                )

                            [1] => 
                            [2] => 
                            [3] => 
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
                            [0] => 1
                        )

                )

            [13] => Handlebars\Opcode Object
                (
                    [opcode] => emptyHash
                    [args] => Array
                        (
                            [0] => 
                        )

                )

            [14] => Handlebars\Opcode Object
                (
                    [opcode] => invokeKnownHelper
                    [args] => Array
                        (
                            [0] => 1
                            [1] => if
                        )

                )

            [15] => Handlebars\Opcode Object
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
                                            [0] =>   <ul>

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
                                                    [0] => items
                                                )

                                            [1] => 
                                            [2] => 
                                            [3] => 
                                        )

                                )

                            [3] => Handlebars\Opcode Object
                                (
                                    [opcode] => pushProgram
                                    [args] => Array
                                        (
                                            [0] => 0
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
                                            [0] => 
                                        )

                                )

                            [6] => Handlebars\Opcode Object
                                (
                                    [opcode] => invokeKnownHelper
                                    [args] => Array
                                        (
                                            [0] => 1
                                            [1] => each
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
                                            [0] =>   </ul>

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
                                                                    [0] => current
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
                                                            [0] => 1
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
                                                                            [0] =>         <li><strong>
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
                                                                                    [0] => name
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
                                                                            [0] => name
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
                                                                            [0] => </strong></li>

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

                                            [1] => Handlebars\Program Object
                                                (
                                                    [opcodes] => Array
                                                        (
                                                            [0] => Handlebars\Opcode Object
                                                                (
                                                                    [opcode] => appendContent
                                                                    [args] => Array
                                                                        (
                                                                            [0] =>         <li><a href="
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
                                                                                    [0] => url
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
                                                                            [0] => url
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
                                                                            [0] => ">
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
                                                                                    [0] => name
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
                                                                            [0] => name
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
                                                                            [0] => </a></li>

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

            [1] => Handlebars\Program Object
                (
                    [opcodes] => Array
                        (
                            [0] => Handlebars\Opcode Object
                                (
                                    [opcode] => appendContent
                                    [args] => Array
                                        (
                                            [0] =>   <p>The list is empty.</p>

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