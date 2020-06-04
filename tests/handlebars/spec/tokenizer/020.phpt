--TEST--
Tokenizer - tokenizes a path with .. as a parent path
--DESCRIPTION--
Tokenizer - tokenizes a path with .. as a parent path
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
$tmpl = '{{../foo.bar}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [..] SEP [/] ID [foo] SEP [.] ID [bar] CLOSE [}}]
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
            [text] => ..
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
            [name] => SEP
            [text] => .
        )

    [5] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bar
        )

    [6] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
