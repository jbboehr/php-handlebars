--TEST--
track ids - builtin helpers - #blockHelperMissing - should handle nesting
--DESCRIPTION--
track ids - builtin helpers - #blockHelperMissing - should handle nesting
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
$tmpl = '{{#bat}}{{#field}}{{wycats name}}{{/field}}{{/bat}}';
$context = array(
        'bat' => array(
            'field' => array(
                'name' => 'foo',
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
foo:bat.field
