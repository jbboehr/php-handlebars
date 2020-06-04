--TEST--
parser - parses mustaches with BOOLEAN parameters - 01
--DESCRIPTION--
parser - parses mustaches with BOOLEAN parameters - 01
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
$tmpl = '{{foo false}}';

try {
    var_export(Parser::parsePrint($tmpl));
    var_export(gettype(Parser::parse($tmpl)));
} catch( Handlebars\ParseException $e ) {
    echo "exception: ", $e->getMessage();
}

echo PHP_EOL;

--EXPECT--
'{{ PATH:foo [BOOLEAN{false}] }}''array'