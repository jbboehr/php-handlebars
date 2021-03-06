--TEST--
Tokenizer - tokenizes block params
--DESCRIPTION--
Tokenizer - tokenizes block params
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
$tmpl = '{{#foo as |bar|}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN_BLOCK [{{#] ID [foo] OPEN_BLOCK_PARAMS [as |] ID [bar] CLOSE_BLOCK_PARAMS [|] CLOSE [}}]
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
            [name] => CLOSE_BLOCK_PARAMS
            [text] => |
        )

    [5] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
