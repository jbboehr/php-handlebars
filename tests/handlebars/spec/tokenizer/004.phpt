--TEST--
Tokenizer - supports escaping delimiters
--DESCRIPTION--
Tokenizer - supports escaping delimiters
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
$tmpl = '{{foo}} \\{{bar}} {{baz}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN [{{] ID [foo] CLOSE [}}] CONTENT [ ] CONTENT [{{bar}} ] OPEN [{{] ID [baz] CLOSE [}}]
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
            [text] =>  
        )

    [4] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] => {{bar}} 
        )

    [5] => Handlebars\Token Object
        (
            [name] => OPEN
            [text] => {{
        )

    [6] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [7] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
