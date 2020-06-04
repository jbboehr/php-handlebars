--TEST--
data - deep @foo triggers automatic top-level data
--DESCRIPTION--
data - deep @foo triggers automatic top-level data
Automatic data was triggered
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
$tmpl = '{{#let world="world"}}{{#if foo}}{{#if foo}}Hello {{@world}}{{/if}}{{/if}}{{/let}}';
$context = array(
        'foo' => true,
    );
$helpers = new DefaultRegistry(array(
        'let' => function($options) {
        $frame = Utils::createFrame($options['data']);
        foreach( $options['hash'] as $prop => $v ) {
          $frame[$prop] = $options['hash'][$prop];
        }
        return $options->fn($options->scope, array("data" => $frame));
      }/*function (options) {
        var frame = Handlebars.createFrame(options.data);
        for (var prop in options.hash) {
          if (prop in options.hash) {
            frame[prop] = options.hash[prop];
          }
        }
        return options.fn(this, {data: frame});
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Hello world