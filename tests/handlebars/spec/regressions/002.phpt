--TEST--
Regressions - GH-150: Inverted sections print when they shouldn't
--DESCRIPTION--
Regressions - GH-150: Inverted sections print when they shouldn't
inverted sections run when property isn't present in context
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
$tmpl = '{{^set}}not set{{/set}} :: {{#set}}set{{/set}}';
$context = array();
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
not set :: 