--TEST--
basic context - block functions without context argument
--DESCRIPTION--
basic context - block functions without context argument
block functions are called with options
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
$tmpl = '{{#awesome}}inner{{/awesome}}';
$context = array(
        'awesome' => function($options) {
        return $options->fn($options->scope);
      }/*function (options) {
        return options.fn(this);
      }*/,
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
inner