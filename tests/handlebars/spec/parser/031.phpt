--TEST--
parser - parses mustaches with hash arguments - 05
--DESCRIPTION--
parser - parses mustaches with hash arguments - 05
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
$tmpl = '{{foo bar=baz bat=bam}}';

try {
    var_export(Parser::parsePrint($tmpl));
    var_export(gettype(Parser::parse($tmpl)));
} catch( Handlebars\ParseException $e ) {
    echo "exception: ", $e->getMessage();
}

echo PHP_EOL;

--EXPECT--
'{{ PATH:foo [] HASH{bar=PATH:baz, bat=PATH:bam} }}''array'