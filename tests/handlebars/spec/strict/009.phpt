--TEST--
strict - strict mode - should not run helperMissing for helper calls
--DESCRIPTION--
strict - strict mode - should not run helperMissing for helper calls
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
$tmpl = '{{hello foo}}';
$context = array(
        'foo' => true,
    );
$compileOptions = array(
        'strict' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'strict' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECTF--
%AUncaught%A