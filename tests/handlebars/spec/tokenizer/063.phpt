--TEST--
Tokenizer - tokenizes hash arguments - 08
--DESCRIPTION--
Tokenizer - tokenizes hash arguments - 08
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
$tmpl = '{{foo omg bar=baz bat="bam"}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] ID [omg] ID [bar] EQUALS [=] ID [baz] ID [bat] EQUALS [=] STRING [bam] CLOSE [}}]
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
            [text] => omg
        )

    [3] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bar
        )

    [4] => Handlebars\Token Object
        (
            [name] => EQUALS
            [text] => =
        )

    [5] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [6] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bat
        )

    [7] => Handlebars\Token Object
        (
            [name] => EQUALS
            [text] => =
        )

    [8] => Handlebars\Token Object
        (
            [name] => STRING
            [text] => bam
        )

    [9] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
