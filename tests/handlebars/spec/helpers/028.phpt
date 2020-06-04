--TEST--
helpers - helpers hash - in cases of conflict, helpers win
--DESCRIPTION--
helpers - helpers hash - in cases of conflict, helpers win
helpers hash has precedence escaped expansion
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
$tmpl = '{{{lookup}}}';
$context = array(
        'lookup' => 'Explicit',
    );
$helpers = new DefaultRegistry(array(
        'lookup' => function() { return 'helpers'; }/*function () { return 'helpers'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
helpers