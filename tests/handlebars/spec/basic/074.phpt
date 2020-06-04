--TEST--
basic context - this keyword in helpers
--DESCRIPTION--
basic context - this keyword in helpers
This keyword in paths evaluates to current context
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
$tmpl = '{{#goodbyes}}{{foo this}}{{/goodbyes}}';
$context = array(
        'goodbyes' => array(
            'goodbye',
            'Goodbye',
            'GOODBYE',
        ),
    );
$helpers = new DefaultRegistry(array(
        'foo' => function($value) { return 'bar ' . $value; }/*function (value) {
        return 'bar ' + value;
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
bar goodbyebar Goodbyebar GOODBYE