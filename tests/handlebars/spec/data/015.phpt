--TEST--
data - passing in data to a compiled function that expects data - works with block helpers that use ..
--DESCRIPTION--
data - passing in data to a compiled function that expects data - works with block helpers that use ..
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
$tmpl = '{{#hello}}{{world ../zomg}}{{/hello}}';
$context = array(
        'exclaim' => true,
        'zomg' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'hello' => function($options) {
        return $options->fn(array("exclaim" => "?"));
      }/*function (options) {
        return options.fn({exclaim: '?'});
      }*/,
        'world' => function($thing, $options) {
        return $options['data']['adjective'] . " " . $thing . ($options->scope['exclaim'] ?: "");
      }/*function (thing, options) {
        return options.data.adjective + ' ' + thing + (this.exclaim || '');
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
happy world?