--TEST--
builtin helpers - #with - with with function argument
--DESCRIPTION--
builtin helpers - #with - with with function argument
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
$tmpl = '{{#with person}}{{first}} {{last}}{{/with}}';
$context = array(
        'person' => function() { return array("first" => "Alan", "last" => "Johnson"); }/*function () { return {first: 'Alan', last: 'Johnson'}; }*/,
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Alan Johnson