--TEST--
helpers - helper with complex lookup$
--DESCRIPTION--
helpers - helper with complex lookup$
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
$tmpl = '{{#goodbyes}}{{{link ../prefix}}}{{/goodbyes}}';
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
        'link' => function($prefix, $options) { return '<a href="' . $prefix . '/' . $options->scope['url'] . '">' . $options->scope['text'] . '</a>'; }/*function (prefix) {
      return '<a href="' + prefix + '/' + this.url + '">' + this.text + '</a>';
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