--TEST--
Dotted Names - Broken Chain Resolution - Each part of a dotted name should resolve only against its parent.
--DESCRIPTION--
Dotted Names - Broken Chain Resolution - Each part of a dotted name should resolve only against its parent.
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
require __DIR__ . "/../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '"{{a.b.c.name}}" == ""';
$context = array(
        'a' => array(
            'b' => array(),
        ),
        'c' => array(
            'name' => 'Jim',
        ),
    );
$compileOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
"" == ""