--TEST--
helpers - raw block parsing (with identity helper-function) - helper for nested raw block closes after first matching close
--DESCRIPTION--
helpers - raw block parsing (with identity helper-function) - helper for nested raw block closes after first matching close
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
$tmpl = '{{{{identity}}}}abc{{{{/identity}}}} {{{{identity}}}}abc{{{{/identity}}}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'identity' => function($options) {
          return $options->fn();
      }/*function (options) {
          return options.fn();
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
abc abc