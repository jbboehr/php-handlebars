--TEST--
helpers - name conflicts - Scoped names take precedence over block helpers
--DESCRIPTION--
helpers - name conflicts - Scoped names take precedence over block helpers
Helper executed
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
$tmpl = '{{#goodbye}} {{cruel world}}{{/goodbye}} {{this.goodbye}}';
$context = array(
        'goodbye' => 'goodbye',
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'goodbye' => function($options) {
          return strtoupper($options->scope['goodbye']) . $options->fn($options->scope);
        }/*function (options) {
          return this.goodbye.toUpperCase() + options.fn(this);
        }*/,
        'cruel' => function($world) {
          return "cruel " . strtoupper($world);
        }/*function (world) {
          return 'cruel ' + world.toUpperCase();
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE cruel WORLD goodbye