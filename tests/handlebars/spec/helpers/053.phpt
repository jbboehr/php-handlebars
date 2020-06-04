--TEST--
helpers - knownHelpers - Unknown helper in knownHelpers only mode should be passed as undefined
--DESCRIPTION--
helpers - knownHelpers - Unknown helper in knownHelpers only mode should be passed as undefined
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
$tmpl = '{{typeof hello}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'typeof' => function($arg) { return $arg === null ? 'undefined' : gettype($arg); }/*function (arg) { return typeof arg; }*/,
        'hello' => function() {
          return 'foo';
        }/*function () {
          return 'foo';
        }*/,
    ));
$compileOptions = array(
        'knownHelpers' => array(
            'typeof' => true,
        ),
        'knownHelpersOnly' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'knownHelpers' => array(
            'typeof' => true,
        ),
        'knownHelpersOnly' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
undefined