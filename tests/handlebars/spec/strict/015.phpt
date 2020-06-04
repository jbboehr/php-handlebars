--TEST--
strict - strict mode - should allow undefined hash when passed to helpers
--DESCRIPTION--
strict - strict mode - should allow undefined hash when passed to helpers
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
$tmpl = '{{helper value=@foo}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'helper' => function () {
          return 'success';
        }/*function (options) {
          equals('value' in options.hash, true);
          equals(options.hash.value, undefined);
          return 'success';
        }*/,
    ));
$compileOptions = array(
        'strict' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'strict' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
success