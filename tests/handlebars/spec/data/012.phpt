--TEST--
data - passing in data to a compiled function that expects data - works with helpers in partials
--DESCRIPTION--
data - passing in data to a compiled function that expects data - works with helpers in partials
Data output by helper inside partial
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
$tmpl = '{{>myPartial}}';
$context = array(
        'noun' => 'cat',
    );
$helpers = new DefaultRegistry(array(
        'hello' => function($options) {
        return $options['data']['adjective'] . " "  . $options->scope['noun'];
      }/*function (options) {
        return options.data.adjective + ' ' + this.noun;
      }*/,
    ));
$partials = new DefaultRegistry(array(
        'myPartial' => '{{hello}}',
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
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
happy cat