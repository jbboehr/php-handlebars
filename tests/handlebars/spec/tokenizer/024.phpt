--TEST--
Tokenizer - tokenizes raw content as "CONTENT"
--DESCRIPTION--
Tokenizer - tokenizes raw content as "CONTENT"
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
$tmpl = 'foo {{ bar }} baz';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
CONTENT [foo ] OPEN [{{] ID [bar] CLOSE [}}] CONTENT [ baz]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] => foo 
        )

    [1] => Handlebars\Token Object
        (
            [name] => OPEN
            [text] => {{
        )

    [2] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bar
        )

    [3] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

    [4] => Handlebars\Token Object
        (
            [name] => CONTENT
            [text] =>  baz
        )

)
