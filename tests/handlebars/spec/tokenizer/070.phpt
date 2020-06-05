--TEST--
Tokenizer - tokenizes subexpressions - 01
--DESCRIPTION--
Tokenizer - tokenizes subexpressions - 01
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
$tmpl = '{{foo (a-x b-y)}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] OPEN_SEXPR [(] ID [a-x] ID [b-y] CLOSE_SEXPR [)] CLOSE [}}]
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
            [text] => a-x
        )

    [4] => Handlebars\Token Object
        (
            [name] => ID
            [text] => b-y
        )

    [5] => Handlebars\Token Object
        (
            [name] => CLOSE_SEXPR
            [text] => )
        )

    [6] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
