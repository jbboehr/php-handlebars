--TEST--
track ids - should include hash ids
--DESCRIPTION--
track ids - should include hash ids
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
$tmpl = '{{wycats bat=is.a baz=slave.driver}}';
$context = array(
        'is' => array(
            'a' => 'foo',
        ),
        'slave' => array(
            'driver' => 'bar',
        ),
    );
$helpers = new DefaultRegistry(array(
        'wycats' => function($options) {
        return "HELP ME MY BOSS " . $options['hashIds']['bat'] . ':' . $options['hash']['bat'] . ' ' . $options['hashIds']['baz'] . ':' . $options['hash']['baz'];
      }/*function (options) {
        equal(options.hashIds.bat, 'is.a');
        equal(options.hashIds.baz, 'slave.driver');

        return 'HELP ME MY BOSS ' + options.hashIds.bat + ':' + options.hash.bat + ' ' + options.hashIds.baz + ':' + options.hash.baz;
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
HELP ME MY BOSS is.a:foo slave.driver:bar