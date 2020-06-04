--TEST--
Tokenizer - tokenizes a partial space at the }); as "OPEN_PARTIAL ID CLOSE" - 01
--DESCRIPTION--
Tokenizer - tokenizes a partial space at the }); as "OPEN_PARTIAL ID CLOSE" - 01
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
$tmpl = '{{>foo/bar.baz  }}';
myprint(Tokenizer::lexPrint($tmpl), true);
echo PHP_EOL;
myprint(Tokenizer::lex($tmpl), true);
--EXPECT--
OPEN_PARTIAL [{{>] ID [foo] SEP [/] ID [bar] SEP [.] ID [baz] CLOSE [}}]
Array
(
    [0] => Handlebars\Token Object
        (
            [name] => OPEN_PARTIAL
            [text] => {{>
        )

    [1] => Handlebars\Token Object
        (
            [name] => ID
            [text] => foo
        )

    [2] => Handlebars\Token Object
        (
            [name] => SEP
            [text] => /
        )

    [3] => Handlebars\Token Object
        (
            [name] => ID
            [text] => bar
        )

    [4] => Handlebars\Token Object
        (
            [name] => SEP
            [text] => .
        )

    [5] => Handlebars\Token Object
        (
            [name] => ID
            [text] => baz
        )

    [6] => Handlebars\Token Object
        (
            [name] => CLOSE
            [text] => }}
        )

)
