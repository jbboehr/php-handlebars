--TEST--
parser - parses non-empty blocks with empty inverse section
--DESCRIPTION--
parser - parses non-empty blocks with empty inverse section
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
$tmpl = '{{#foo}} bar {{^}}{{/foo}}';

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
    CONTENT[ \' bar \' ]
  {{^}}''array'