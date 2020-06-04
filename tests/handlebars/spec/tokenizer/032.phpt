--TEST--
Tokenizer - tokenizes a block comment as "COMMENT"
--DESCRIPTION--
Tokenizer - tokenizes a block comment as "COMMENT"
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
$tmpl = 'foo {{!-- this is a {{comment}} --}} bar {{ baz }}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
CONTENT [foo ] COMMENT [{{!-- this is a {{comment}} --}}] CONTENT [ bar ] OPEN [{{] ID [baz] CLOSE [}}]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] => foo 
        )

    [1] => Handlebars\Token Object
        (
            [name] => COMMENT
            [text] => {{!-- this is a {{comment}} --}}
        )

    [2] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] =>  bar 
        )

    [3] => Handlebars\Token Object
        (
            [name] => OPEN
            [text] => {{
        )

    [4] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [5] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
