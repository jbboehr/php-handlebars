--TEST--
blocks - decorators - should access parent variables
--DESCRIPTION--
blocks - decorators - should access parent variables
--SKIPIF--
<?php
if( true ) die('skip decorators are not supported by the VM');
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
$tmpl = '{{#helper}}{{*decorator foo}}{{/helper}}';
$context = array(
        'foo' => 'success',
    );
$helpers = new DefaultRegistry(array(
        'helper' => function($options) {
          return $options->fn->run;
        }/*function (options) {
          return options.fn.run;
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
success