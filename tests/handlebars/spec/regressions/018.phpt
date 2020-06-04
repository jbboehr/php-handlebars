--TEST--
Regressions - GH-731: zero context rendering
--DESCRIPTION--
Regressions - GH-731: zero context rendering
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
$tmpl = '{{#foo}} This is {{bar}} ~ {{/foo}}';
$context = array(
        'foo' => 0,
        'bar' => 'OK',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
 This is  ~ 