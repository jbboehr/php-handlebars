--TEST--
basic context - functions returning safestrings shouldn't be escaped
--DESCRIPTION--
basic context - functions returning safestrings shouldn't be escaped
functions returning safestrings aren't escaped
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
$tmpl = '{{awesome}}';
$context = array(
        'awesome' => function () { return new SafeString('&\'\\<>'); }/*function () { return new Handlebars.SafeString('&\'\\<>'); }*/,
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
&'\<>