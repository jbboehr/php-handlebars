--TEST--
Tokenizer - supports escaping escape character
--DESCRIPTION--
Tokenizer - supports escaping escape character
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
$tmpl = '{{foo}} \\\\{{bar}} {{baz}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] CLOSE [}}] CONTENT [ \] OPEN [{{] ID [bar] CLOSE [}}] CONTENT [ ] OPEN [{{] ID [baz] CLOSE [}}]
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
            [name] => CLOSE
            [text] => }}
        )

    [3] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] =>  \
        )

    [4] => Handlebars\Token Object
        (
            [name] => OPEN
            [text] => {{
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

    [7] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] =>  
        )

    [8] => Handlebars\Token Object
        (
            [name] => OPEN
            [text] => {{
        )

    [9] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [10] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
