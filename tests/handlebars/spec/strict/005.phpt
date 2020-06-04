--TEST--
strict - strict mode - should error on missing property lookup in known helpers mode
--DESCRIPTION--
strict - strict mode - should error on missing property lookup in known helpers mode
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
$compileOptions = array(
        'strict' => true,
        'knownHelpersOnly' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'strict' => true,
        'knownHelpersOnly' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECTF--
%AUncaught%A