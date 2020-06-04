--TEST--
blocks - standalone sections - should handle nesting
--DESCRIPTION--
blocks - standalone sections - should handle nesting
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
$tmpl = '{{#data}}
{{#if true}}
{{.}}
{{/if}}
{{/data}}
OK.';
$context = array(
        'data' => array(
            1,
            3,
            5,
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
1
3
5
OK.