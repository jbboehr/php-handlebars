--TEST--
helpers - knownHelpers - Known helper should render helper
--DESCRIPTION--
helpers - knownHelpers - Known helper should render helper
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
$tmpl = '{{hello}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'hello' => function() {
          return 'foo';
        }/*function () {
          return 'foo';
        }*/,
    ));
$compileOptions = array(
        'knownHelpers' => array(
            'hello' => true,
        ),
    );
$runtimeOptions = array();
$allOptions = array(
        'knownHelpers' => array(
            'hello' => true,
        ),
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
foo