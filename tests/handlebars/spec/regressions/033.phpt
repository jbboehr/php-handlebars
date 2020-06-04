--TEST--
Regressions - GH-1639: TypeError: Cannot read property 'apply' of undefined" when handlebars version > 4.6.0 (undocumented, deprecated usage) - should treat undefined helpers like non-existing helpers
--DESCRIPTION--
Regressions - GH-1639: TypeError: Cannot read property 'apply' of undefined" when handlebars version > 4.6.0 (undocumented, deprecated usage) - should treat undefined helpers like non-existing helpers
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
$tmpl = '{{foo}}';
$context = array(
        'foo' => 'bar',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
bar