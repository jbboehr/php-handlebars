--TEST--
Recursion - The greater-than operator should properly recurse.
--DESCRIPTION--
Recursion - The greater-than operator should properly recurse.
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
$tmpl = '{{>node}}';
$context = array(
        'content' => 'X',
        'nodes' => array(
            array(
                'content' => 'Y',
                'nodes' => array(),
            ),
        ),
    );
$partials = new DefaultRegistry(array(
        'node' => '{{content}}<{{#nodes}}{{>node}}{{/nodes}}>',
    ));
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
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
X<Y<>>