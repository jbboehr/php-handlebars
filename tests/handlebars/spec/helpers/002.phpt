--TEST--
helpers - helper for raw block gets raw content
--DESCRIPTION--
helpers - helper for raw block gets raw content
raw block helper gets raw content
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
$tmpl = '{{{{raw}}}} {{test}} {{{{/raw}}}}';
$context = array(
        'test' => 'hello',
    );
$helpers = new DefaultRegistry(array(
        'raw' => function($options) {
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
 {{test}} 