--TEST--
parser - parses multiple inverse sections
--DESCRIPTION--
parser - parses multiple inverse sections
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
$tmpl = '{{#foo}} bar {{else if bar}}{{else}} baz {{/foo}}';

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
  {{^}}
    BLOCK:
      PATH:if [PATH:bar]
      PROGRAM:
      {{^}}
        CONTENT[ \' baz \' ]''array'