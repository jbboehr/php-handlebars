--TEST--
Tokenizer - tokenizes a path with this/foo as OPEN ID SEP ID CLOSE
--DESCRIPTION--
Tokenizer - tokenizes a path with this/foo as OPEN ID SEP ID CLOSE
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
$tmpl = '{{this/foo}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [this] SEP [/] ID [foo] CLOSE [}}]
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
            [text] => this
        )

    [2] => Handlebars\Token Object
        (
            [name] => SEP
            [text] => /
        )

    [3] => Handlebars\Token Object
        (
            [name] => ID
            [text] => foo
        )

    [4] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
