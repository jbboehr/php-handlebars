--TEST--
data - passing in data to a compiled function that expects data - works with block helpers
--DESCRIPTION--
data - passing in data to a compiled function that expects data - works with block helpers
Data output by helper
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
$tmpl = '{{#hello}}{{world}}{{/hello}}';
$context = array(
        'exclaim' => true,
    );
$helpers = new DefaultRegistry(array(
        'hello' => function($options) {
        return $options->fn($options->scope);
      }/*function (options) {
        return options.fn(this);
      }*/,
        'world' => function($options) {
        return $options['data']['adjective'] . " world" . ($options->scope['exclaim'] ? "!" : "");
      }/*function (options) {
        return options.data.adjective + ' world' + (this.exclaim ? '!' : '');
      }*/,
    ));
$compileOptions = array(
        'data' => true,
    );
$runtimeOptions = array(
        'data' => array(
            'adjective' => 'happy',
        ),
    );
$allOptions = array(
        'data' => array(
            'adjective' => 'happy',
        ),
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
happy world!