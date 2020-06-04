--TEST--
Tokenizer - supports unescaping with {{{
--DESCRIPTION--
Tokenizer - supports unescaping with {{{
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
$tmpl = '{{{bar}}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN_UNESCAPED [{{{] ID [bar] CLOSE_UNESCAPED [}}}]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => OPEN_UNESCAPED
            [text] => {{{
        )

    [1] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bar
        )

    [2] => Handlebars\Token Object
        (
            [name] => CLOSE_UNESCAPED
            [text] => }}}
        )

)
