--TEST--
track ids - builtin helpers - #each - should handle block params
--DESCRIPTION--
track ids - builtin helpers - #each - should handle block params
--SKIPIF--
<?php
if( true ) die('skip track ids are not supported by the VM');
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
$tmpl = '{{#each array as |value|}}{{blockParams value.name}}{{/each}}';
$context = array(
        'array' => array(
            array(
                'name' => 'foo',
            ),
            array(
                'name' => 'bar',
            ),
        ),
    );
$helpers = new DefaultRegistry(array(
        'blockParams' => function($name, $options) {
        return $name . ':' . $options->ids[0] . "\n";
      }/*function (name, options) {
        return name + ':' + options.ids[0] + '\n';
      }*/,
        'wycats' => function($name, $options) {
        return $name . ':' . $options['data']['contextPath'] . "\n";
      }/*function (name, options) {
        return name + ':' + options.data.contextPath + '\n';
      }*/,
    ));
$compileOptions = array(
        'trackIds' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'trackIds' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
foo:array.0.name
bar:array.1.name
