--TEST--
List - Lists should be iterated; list items should visit the context stack.
--DESCRIPTION--
List - Lists should be iterated; list items should visit the context stack.
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
require __DIR__ . "/../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '"{{#list}}{{item}}{{/list}}"';
$context = array(
        'list' => array(
            array(
                'item' => 1,
            ),
            array(
                'item' => 2,
            ),
            array(
                'item' => 3,
            ),
        ),
    );
$compileOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
"123"