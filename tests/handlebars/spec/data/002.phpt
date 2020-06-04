--TEST--
data - data can be looked up via @foo
--DESCRIPTION--
data - data can be looked up via @foo
@foo retrieves template data
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
$tmpl = '{{@hello}}';
$context = array();
$compileOptions = array();
$runtimeOptions = array(
        'data' => array(
            'hello' => 'hello',
        ),
    );
$allOptions = array(
        'data' => array(
            'hello' => 'hello',
        ),
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
hello