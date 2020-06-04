--TEST--
Tokenizer - tokenizes hash arguments - 02
--DESCRIPTION--
Tokenizer - tokenizes hash arguments - 02
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
$tmpl = '{{ foo bar baz=1 }}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] ID [bar] ID [baz] EQUALS [=] NUMBER [1] CLOSE [}}]
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
            [name] => ID
            [text] => bar
        )

    [3] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [4] => Handlebars\Token Object
        (
            [name] => EQUALS
            [text] => =
        )

    [5] => Handlebars\Token Object
        (
            [name] => NUMBER
            [text] => 1
        )

    [6] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
