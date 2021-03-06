--TEST--
strict - assume objects - should error on missing object
--DESCRIPTION--
strict - assume objects - should error on missing object
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
$tmpl = '{{hello.bar}}';
$context = array();
$compileOptions = array(
        'assumeObjects' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'assumeObjects' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECTF--
%AUncaught%A