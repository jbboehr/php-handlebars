--TEST--
helpers - block helper staying in the same context
--DESCRIPTION--
helpers - block helper staying in the same context
Block helper executed with current context
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
$tmpl = '{{#form}}<p>{{name}}</p>{{/form}}';
$context = array(
        'name' => 'Yehuda',
    );
$helpers = new DefaultRegistry(array(
        'form' => function($options) { $out = '<form>'; $out .= $options->fn($options->scope); return $out . '</form>'; }/*function (options) { return '<form>' + options.fn(this) + '</form>'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<form><p>Yehuda</p></form>