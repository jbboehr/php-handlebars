--TEST--
Tokenizer - tokenizes inverse sections as "INVERSE"
--DESCRIPTION--
Tokenizer - tokenizes inverse sections as "INVERSE"
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
$tmpl = '{{^}}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
INVERSE [{{^}}]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => INVERSE
            [text] => {{^}}
        )

)
