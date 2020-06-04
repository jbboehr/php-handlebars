--TEST--
helpers - helper with complex lookup and nested template
--DESCRIPTION--
helpers - helper with complex lookup and nested template
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
$tmpl = '{{#goodbyes}}{{#link ../prefix}}{{text}}{{/link}}{{/goodbyes}}';
$context = array(
        'prefix' => '/root',
        'goodbyes' => array(
            array(
                'text' => 'Goodbye',
                'url' => 'goodbye',
            ),
        ),
    );
$helpers = new DefaultRegistry(array(
        'link' => function($prefix, $options) { $out = '<a href="' . $prefix . '/' . $options->scope['url'] . '">'; $out .= $options->fn($options->scope); return $out . '</a>'; }/*function (prefix, options) {
        return '<a href="' + prefix + '/' + this.url + '">' + options.fn(this) + '</a>';
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<a href="/root/goodbye">Goodbye</a>