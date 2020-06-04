--TEST--
helpers - nested block helpers
--DESCRIPTION--
helpers - nested block helpers
Both blocks executed
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
$tmpl = '{{#form yehuda}}<p>{{name}}</p>{{#link}}Hello{{/link}}{{/form}}';
$context = array(
        'yehuda' => array(
            'name' => 'Yehuda',
        ),
    );
$helpers = new DefaultRegistry(array(
        'link' => function($options) {
    $out = '<a href="' . $options->scope['name'].'">';
    $out .= $options->fn($options->scope);
    return $out . '</a>';
}/*function (options) { return '<a href="' + this.name + '">' + options.fn(this) + '</a>'; }*/,
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
<form><p>Yehuda</p><a href="Yehuda">Hello</a></form>