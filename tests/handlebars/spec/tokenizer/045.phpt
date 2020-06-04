--TEST--
Tokenizer - tokenizes String params with spaces inside as "STRING"
--DESCRIPTION--
Tokenizer - tokenizes String params with spaces inside as "STRING"
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
$tmpl = '{{ foo bar "baz bat" }}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] ID [bar] STRING [baz bat] CLOSE [}}]
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
            [name] => STRING
            [text] => baz bat
        )

    [4] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
