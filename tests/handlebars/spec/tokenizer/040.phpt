--TEST--
Tokenizer - tokenizes inverse sections with ID as "OPEN_INVERSE ID CLOSE"
--DESCRIPTION--
Tokenizer - tokenizes inverse sections with ID as "OPEN_INVERSE ID CLOSE"
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
$tmpl = '{{^foo}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN_INVERSE [{{^] ID [foo] CLOSE [}}]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => OPEN_INVERSE
            [text] => {{^
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

)
