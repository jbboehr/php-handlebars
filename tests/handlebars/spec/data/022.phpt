--TEST--
data - nesting - the root context can be looked up via @root
--DESCRIPTION--
data - nesting - the root context can be looked up via @root
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
$tmpl = '{{#helper}}{{#helper}}{{@./depth}} {{@../depth}} {{@../../depth}}{{/helper}}{{/helper}}';
$context = array(
        'foo' => 'hello',
    );
$helpers = new DefaultRegistry(array(
        'helper' => function($options) {
            $frame = Utils::createFrame($options['data']);
            $frame['depth'] = $options['data']['depth'] + 1;
            return $options->fn($options->scope, array("data" => $frame));
          }/*function (options) {
            var frame = Handlebars.createFrame(options.data);
            frame.depth = options.data.depth + 1;
            return options.fn(this, {data: frame});
          }*/,
    ));
$compileOptions = array();
$runtimeOptions = array(
        'data' => array(
            'depth' => 0,
        ),
    );
$allOptions = array(
        'data' => array(
            'depth' => 0,
        ),
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
2 1 0