--TEST--
Tokenizer - tokenizes nested subexpressions: literals
--DESCRIPTION--
Tokenizer - tokenizes nested subexpressions: literals
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
$tmpl = '{{foo (bar (lol true) false) (baz 1) (blah \'b\') (blorg "c")}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] OPEN_SEXPR [(] ID [bar] OPEN_SEXPR [(] ID [lol] BOOLEAN [true] CLOSE_SEXPR [)] BOOLEAN [false] CLOSE_SEXPR [)] OPEN_SEXPR [(] ID [baz] NUMBER [1] CLOSE_SEXPR [)] OPEN_SEXPR [(] ID [blah] STRING [b] CLOSE_SEXPR [)] OPEN_SEXPR [(] ID [blorg] STRING [c] CLOSE_SEXPR [)] CLOSE [}}]
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
            [name] => BOOLEAN
            [text] => true
        )

    [7] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [8] => Handlebars\Token Object
        (
            [name] => BOOLEAN
            [text] => false
        )

    [9] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [10] => Handlebars\Token Object
        (
            [name] => OPEN_SEXPR
            [text] => (
        )

    [11] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [12] => Handlebars\Token Object
        (
            [name] => NUMBER
            [text] => 1
        )

    [13] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [14] => Handlebars\Token Object
        (
            [name] => OPEN_SEXPR
            [text] => (
        )

    [15] => Handlebars\Token Object
        (
            [name] => ID
            [text] => blah
        )

    [16] => Handlebars\Token Object
        (
            [name] => STRING
            [text] => b
        )

    [17] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [18] => Handlebars\Token Object
        (
            [name] => OPEN_SEXPR
            [text] => (
        )

    [19] => Handlebars\Token Object
        (
            [name] => ID
            [text] => blorg
        )

    [20] => Handlebars\Token Object
        (
            [name] => STRING
            [text] => c
        )

    [21] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [22] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
