--TEST--
Tokenizer - tokenizes undefined and null
--DESCRIPTION--
Tokenizer - tokenizes undefined and null
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
$tmpl = '{{ foo undefined null }}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] UNDEFINED [undefined] NULL [null] CLOSE [}}]
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
            [name] => UNDEFINED
            [text] => undefined
        )

    [3] => Handlebars\Token Object
        (
            [name] => NULL
            [text] => null
        )

    [4] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
