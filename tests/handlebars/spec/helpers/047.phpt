--TEST--
helpers - hash - block helpers can take an optional hash with booleans
--DESCRIPTION--
helpers - hash - block helpers can take an optional hash with booleans
Boolean hash parameter honored
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
$tmpl = '{{#goodbye cruel="CRUEL" print=true}}world{{/goodbye}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'goodbye' => function ($options) {
        if ($options['hash']['print'] === true) {
          return 'GOODBYE ' . $options['hash']['cruel'] . ' ' . $options->fn($options->scope);
        } else if ($options['hash']['print'] === false) {
          return 'NOT PRINTING';
        } else {
          return 'THIS SHOULD NOT HAPPEN';
        }
      }/*function goodbye(options) {
        if (options.hash.print === true) {
          return 'GOODBYE ' + options.hash.cruel + ' ' + options.fn(this);
        } else if (options.hash.print === false) {
          return 'NOT PRINTING';
        } else {
          return 'THIS SHOULD NOT HAPPEN';
        }
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE CRUEL world