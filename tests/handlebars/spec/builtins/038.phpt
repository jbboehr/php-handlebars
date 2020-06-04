--TEST--
builtin helpers - #each - data passed to helpers
--DESCRIPTION--
builtin helpers - #each - data passed to helpers
should output data
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
$tmpl = '{{#each letters}}{{this}}{{detectDataInsideEach}}{{/each}}';
$context = array(
        'letters' => array(
            'a',
            'b',
            'c',
        ),
    );
$helpers = new DefaultRegistry(array(
        'detectDataInsideEach' => function($options) { return isset($options['data']['exclaim'])? $options['data']['exclaim'] : ''; }/*function(options) { return options.data && options.data.exclaim; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array(
        'data' => array(
            'exclaim' => '!',
        ),
    );
$allOptions = array(
        'data' => array(
            'exclaim' => '!',
        ),
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
a!b!c!