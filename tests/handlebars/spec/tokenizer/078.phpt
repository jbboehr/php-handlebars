--TEST--
Tokenizer - tokenizes raw blocks
--DESCRIPTION--
Tokenizer - tokenizes raw blocks
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
$tmpl = '{{{{a}}}} abc {{{{/a}}}} aaa {{{{a}}}} abc {{{{/a}}}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN_RAW_BLOCK [{{{{] ID [a] CLOSE_RAW_BLOCK [}}}}] CONTENT [ abc ] END_RAW_BLOCK [a] CONTENT [ aaa ] OPEN_RAW_BLOCK [{{{{] ID [a] CLOSE_RAW_BLOCK [}}}}] CONTENT [ abc ] END_RAW_BLOCK [a]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => OPEN_RAW_BLOCK
            [text] => {{{{
        )

    [1] => Handlebars\Token Object
        (
            [name] => ID
            [text] => a
        )

    [2] => Handlebars\Token Object
        (
            [name] => CLOSE_RAW_BLOCK
            [text] => }}}}
        )

    [3] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] =>  abc 
        )

    [4] => Handlebars\Token Object
        (
            [name] => END_RAW_BLOCK
            [text] => a
        )

    [5] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] =>  aaa 
        )

    [6] => Handlebars\Token Object
        (
            [name] => OPEN_RAW_BLOCK
            [text] => {{{{
        )

    [7] => Handlebars\Token Object
        (
            [name] => ID
            [text] => a
        )

    [8] => Handlebars\Token Object
        (
            [name] => CLOSE_RAW_BLOCK
            [text] => }}}}
        )

    [9] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] =>  abc 
        )

    [10] => Handlebars\Token Object
        (
            [name] => END_RAW_BLOCK
            [text] => a
        )

)
