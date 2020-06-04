--TEST--
Tokenizer - tokenizes block params - 02
--DESCRIPTION--
Tokenizer - tokenizes block params - 02
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
$tmpl = '{{#foo as | bar baz |}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN_BLOCK [{{#] ID [foo] OPEN_BLOCK_PARAMS [as |] ID [bar] ID [baz] CLOSE_BLOCK_PARAMS [|] CLOSE [}}]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => OPEN_BLOCK
            [text] => {{#
        )

    [1] => Handlebars\Token Object
        (
            [name] => ID
            [text] => foo
        )

    [2] => Handlebars\Token Object
        (
            [name] => OPEN_BLOCK_PARAMS
            [text] => as |
        )

    [3] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bar
        )

    [4] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [5] => Handlebars\Token Object
        (
            [name] => CLOSE_BLOCK_PARAMS
            [text] => |
        )

    [6] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
