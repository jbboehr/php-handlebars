--TEST--
Regressions - should support multiple levels of inline partials
--DESCRIPTION--
Regressions - should support multiple levels of inline partials
--SKIPIF--
<?php
if( true ) die('skip inline partials are not supported by the VM');
?>
--FILE--
<?php
use Handlebars\Compiler;
use Handlebars\Parser;
use Handlebars\Tokenizer;
use Handlebars\Utils;
use Handlebars\VM;
require __DIR__ . "/../../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '{{#> layout}}{{#*inline "subcontent"}}subcontent{{/inline}}{{/layout}}';
$context = array();
$partials = new DefaultRegistry(array(
        'doctype' => 'doctype{{> content}}',
        'layout' => '{{#> doctype}}{{#*inline "content"}}layout{{> subcontent}}{{/inline}}{{/doctype}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
doctypelayoutsubcontent