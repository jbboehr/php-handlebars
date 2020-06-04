--TEST--
strict - assume objects - should error on missing context
--DESCRIPTION--
strict - assume objects - should error on missing context
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
$context = NULL;
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