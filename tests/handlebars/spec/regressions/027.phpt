--TEST--
Regressions - GH-1099: should support greater than 3 nested levels of inline partials
--DESCRIPTION--
Regressions - GH-1099: should support greater than 3 nested levels of inline partials
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
$tmpl = '{{#> layout}}Outer{{/layout}}';
$context = array();
$partials = new DefaultRegistry(array(
        'layout' => '{{#> inner}}Inner{{/inner}}{{> @partial-block }}',
        'inner' => '',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Outer