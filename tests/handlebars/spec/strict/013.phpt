--TEST--
strict - strict mode - should throw on ambiguous blocks - 02
--DESCRIPTION--
strict - strict mode - should throw on ambiguous blocks - 02
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
$tmpl = '{{#hello.bar}}{{/hello.bar}}';
$context = array(
        'hello' => array(),
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