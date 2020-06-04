--TEST--
helpers - helpers hash - the helpers hash is available is nested contexts
--DESCRIPTION--
helpers - helpers hash - the helpers hash is available is nested contexts
helpers hash is available in nested contexts.
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
$tmpl = '{{#outer}}{{#inner}}{{helper}}{{/inner}}{{/outer}}';
$context = array(
        'outer' => array(
            'inner' => array(
                'unused' => array(),
            ),
        ),
    );
$helpers = new DefaultRegistry(array(
        'helper' => function() { return 'helper'; }/*function () { return 'helper'; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
helper