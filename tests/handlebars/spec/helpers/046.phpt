--TEST--
helpers - hash - block helpers can take an optional hash with single quoted stings
--DESCRIPTION--
helpers - hash - block helpers can take an optional hash with single quoted stings
Hash parameters output
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
$tmpl = '{{#goodbye cruel="CRUEL" times=12}}world{{/goodbye}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'goodbye' => function($options) {
    return "GOODBYE {$options['hash']['cruel']} " . $options->fn($options->scope) . " {$options['hash']['times']} TIMES";
}/*function (options) {
          return 'GOODBYE ' + options.hash.cruel + ' ' + options.fn(this) + ' ' + options.hash.times + ' TIMES';
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE CRUEL world 12 TIMES