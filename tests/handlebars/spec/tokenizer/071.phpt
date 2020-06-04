--TEST--
Tokenizer - tokenizes nested subexpressions
--DESCRIPTION--
Tokenizer - tokenizes nested subexpressions
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
$tmpl = '{{foo (bar (lol rofl)) (baz)}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] OPEN_SEXPR [(] ID [bar] OPEN_SEXPR [(] ID [lol] ID [rofl] CLOSE_SEXPR [)] CLOSE_SEXPR [)] OPEN_SEXPR [(] ID [baz] CLOSE_SEXPR [)] CLOSE [}}]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => OPEN
            [text] => {{
        )

    [1] => Handlebars\Token Object
        (
            [name] => ID
            [text] => foo
        )

    [2] => Handlebars\Token Object
        (
            [name] => OPEN_SEXPR
            [text] => (
        )

    [3] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bar
        )

    [4] => Handlebars\Token Object
        (
            [name] => OPEN_SEXPR
            [text] => (
        )

    [5] => Handlebars\Token Object
        (
            [name] => ID
            [text] => lol
        )

    [6] => Handlebars\Token Object
        (
            [name] => ID
            [text] => rofl
        )

    [7] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [8] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [9] => Handlebars\Token Object
        (
            [name] => OPEN_SEXPR
            [text] => (
        )

    [10] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [11] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [12] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
