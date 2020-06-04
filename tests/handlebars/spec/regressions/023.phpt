--TEST--
Regressions - GH-1054: Should handle simple safe string responses
--DESCRIPTION--
Regressions - GH-1054: Should handle simple safe string responses
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

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '{{#wrap}}{{>partial}}{{/wrap}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'wrap' => function($options) {
        return new \Handlebars\SafeString($options->fn());
      }/*function (options) {
        return new Handlebars.SafeString(options.fn());
      }*/,
    ));
$partials = new DefaultRegistry(array(
        'partial' => '{{#wrap}}<partial>{{/wrap}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<partial>