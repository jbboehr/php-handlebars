--TEST--
Tokenizer - allows multiple path literals on a line with []
--DESCRIPTION--
Tokenizer - allows multiple path literals on a line with []
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
$tmpl = '{{foo.[bar]}}{{foo.[baz]}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] SEP [.] ID [[bar]] CLOSE [}}] OPEN [{{] ID [foo] SEP [.] ID [[baz]] CLOSE [}}]
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
            [name] => SEP
            [text] => .
        )

    [3] => Handlebars\Token Object
        (
            [name] => ID
            [text] => [bar]
        )

    [4] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

    [5] => Handlebars\Token Object
        (
            [name] => OPEN
            [text] => {{
        )

    [6] => Handlebars\Token Object
        (
            [name] => ID
            [text] => foo
        )

    [7] => Handlebars\Token Object
        (
            [name] => SEP
            [text] => .
        )

    [8] => Handlebars\Token Object
        (
            [name] => ID
            [text] => [baz]
        )

    [9] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
