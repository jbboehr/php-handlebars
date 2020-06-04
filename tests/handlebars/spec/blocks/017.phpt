--TEST--
blocks - inverted sections - chained inverted sections - 01
--DESCRIPTION--
blocks - inverted sections - chained inverted sections - 01
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
$tmpl = '{{#people}}{{name}}{{else if nothere}}fail{{else unless nothere}}{{none}}{{/people}}';
$context = array(
        'none' => 'No people',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
No people