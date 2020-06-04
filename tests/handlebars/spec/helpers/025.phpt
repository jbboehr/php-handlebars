--TEST--
helpers - pathed lambas with parameters - 01
--DESCRIPTION--
helpers - pathed lambas with parameters - 01
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
$tmpl = '{{hash/helper 1}}';
$context = array(
        'helper' => function() {
        return 'winning';
      }/*function () {
        return 'winning';
      }*/,
        'hash' => array(
            'helper' => function() {
        return 'winning';
      }/*function () {
        return 'winning';
      }*/,
        ),
    );
$helpers = new DefaultRegistry(array(
        './helper' => function() { return 'fail'; }/*function () { return 'fail'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
winning