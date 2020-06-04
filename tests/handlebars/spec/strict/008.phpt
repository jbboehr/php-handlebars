--TEST--
strict - strict mode - should error on missing data lookup - 01
--DESCRIPTION--
strict - strict mode - should error on missing data lookup - 01
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
$compileOptions = array(
        'strict' => true,
    );
$runtimeOptions = array(
        'data' => array(
            'hello' => 'foo',
        ),
    );
$allOptions = array(
        'strict' => true,
        'data' => array(
            'hello' => 'foo',
        ),
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
foo