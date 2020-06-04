--TEST--
parser - parses block with block params
--DESCRIPTION--
parser - parses block with block params
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
$tmpl = '{{#foo as |bar baz|}}content{{/foo}}';

try {
    var_export(Parser::parsePrint($tmpl));
    var_export(gettype(Parser::parse($tmpl)));
} catch( Handlebars\ParseException $e ) {
    echo "exception: ", $e->getMessage();
}

echo PHP_EOL;

--EXPECT--
'BLOCK:
  PATH:foo []
  PROGRAM:
    BLOCK PARAMS: [ bar baz ]
    CONTENT[ \'content\' ]''array'