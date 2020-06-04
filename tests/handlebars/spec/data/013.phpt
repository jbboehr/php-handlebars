--TEST--
data - passing in data to a compiled function that expects data - works with helpers and parameters
--DESCRIPTION--
data - passing in data to a compiled function that expects data - works with helpers and parameters
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
$tmpl = '{{hello world}}';
$context = array(
        'exclaim' => true,
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'hello' => function($noun, $options) {
        return $options['data']['adjective'] . " "  . $noun . ($options->scope['exclaim'] ? "!" : "");
      }/*function (noun, options) {
        return options.data.adjective + ' ' + noun + (this.exclaim ? '!' : '');
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