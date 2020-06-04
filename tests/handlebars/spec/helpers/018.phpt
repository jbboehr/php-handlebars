--TEST--
helpers - block helper passing a new context
--DESCRIPTION--
helpers - block helper passing a new context
Context variable resolved
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
$tmpl = '{{#form yehuda}}<p>{{name}}</p>{{/form}}';
$context = array(
        'yehuda' => array(
            'name' => 'Yehuda',
        ),
    );
$helpers = new DefaultRegistry(array(
        'form' => function($context, $options) {
    $out = '<form>';
    $out .= $options->fn($context);
    return $out . '</form>';
}/*function (context, options) { return '<form>' + options.fn(context) + '</form>'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<form><p>Yehuda</p></form>