--TEST--
Regressions - GH-158: Using array index twice, breaks the template
--DESCRIPTION--
Regressions - GH-158: Using array index twice, breaks the template
it works as expected
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
$tmpl = '{{arr.[0]}}, {{arr.[1]}}';
$context = array(
        'arr' => array(
            1,
            2,
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
1, 2