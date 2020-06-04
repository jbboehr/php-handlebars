--TEST--
partials - inline partials - should render nested inline partials with partial-blocks on different nesting levels
--DESCRIPTION--
partials - inline partials - should render nested inline partials with partial-blocks on different nesting levels
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
$tmpl = '{{#*inline "outer"}}{{#>inner}}<outer-block>{{>@partial-block}}</outer-block>{{/inner}}{{>@partial-block}}{{/inline}}{{#*inline "inner"}}<inner>{{>@partial-block}}</inner>{{/inline}}{{#>outer}}{{value}}{{/outer}}';
$context = array(
        'value' => 'success',
    );
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<inner><outer-block>success</outer-block></inner>success