--TEST--
Tokenizer - tokenizes hash arguments - 07
--DESCRIPTION--
Tokenizer - tokenizes hash arguments - 07
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
$tmpl = '{{ foo bar baz="bat" bam=wot }}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] ID [bar] ID [baz] EQUALS [=] STRING [bat] ID [bam] EQUALS [=] ID [wot] CLOSE [}}]
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
            [name] => STRING
            [text] => bat
        )

    [6] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bam
        )

    [7] => Handlebars\Token Object
        (
            [name] => EQUALS
            [text] => =
        )

    [8] => Handlebars\Token Object
        (
            [name] => ID
            [text] => wot
        )

    [9] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
