--TEST--
track ids - should not include anything without the flag
--DESCRIPTION--
track ids - should not include anything without the flag
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
$tmpl = '{{wycats is.a slave.driver}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'wycats' => function($passiveVoice, $noun, $options) {
        return 'success';
      }/*function (passiveVoice, noun, options) {
        equal(options.ids, undefined);
        equal(options.hashIds, undefined);

        return 'success';
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
success