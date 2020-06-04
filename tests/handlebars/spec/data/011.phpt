--TEST--
data - data is inherited downstream
--DESCRIPTION--
data - data is inherited downstream
data variables are inherited downstream
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
$tmpl = '{{#let foo=1 bar=2}}{{#let foo=bar.baz}}{{@bar}}{{@foo}}{{/let}}{{@foo}}{{/let}}';
$context = array(
        'bar' => array(
            'baz' => 'hello world',
        ),
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
$compileOptions = array(
        'data' => true,
    );
$runtimeOptions = array(
        'data' => array(),
    );
$allOptions = array(
        'data' => array(),
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
2hello world1