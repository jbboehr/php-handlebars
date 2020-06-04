--TEST--
basic context - this keyword in paths
--DESCRIPTION--
basic context - this keyword in paths
This keyword in paths evaluates to current context
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
$tmpl = '{{#goodbyes}}{{this}}{{/goodbyes}}';
$context = array(
        'goodbyes' => array(
            'goodbye',
            'Goodbye',
            'GOODBYE',
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
goodbyeGoodbyeGOODBYE