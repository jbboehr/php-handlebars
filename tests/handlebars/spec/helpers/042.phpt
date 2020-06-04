--TEST--
helpers - hash - helpers can take an optional hash
--DESCRIPTION--
helpers - hash - helpers can take an optional hash
Helper output hash
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
$tmpl = '{{goodbye cruel="CRUEL" world="WORLD" times=12}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'goodbye' => function($options) {
    $r = 'GOODBYE ';
    $r .= $options['hash']['cruel'].' ';
    $r .= $options['hash']['world'].' ';
    $r .= $options['hash']['times'].' TIMES';

    return $r;
}/*function (options) {
          return 'GOODBYE ' + options.hash.cruel + ' ' + options.hash.world + ' ' + options.hash.times + ' TIMES';
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE CRUEL WORLD 12 TIMES