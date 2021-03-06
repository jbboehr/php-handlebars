--TEST--
blocks - standalone sections - block standalone else sections can be disabled - 01
--DESCRIPTION--
blocks - standalone sections - block standalone else sections can be disabled - 01
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
$tmpl = '{{#none}}
{{.}}
{{^}}
Fail
{{/none}}
';
$context = array(
        'none' => 'No people',
    );
$compileOptions = array(
        'ignoreStandalone' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'ignoreStandalone' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--

No people

