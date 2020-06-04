--TEST--
builtin helpers - #with - works when data is disabled
--DESCRIPTION--
builtin helpers - #with - works when data is disabled
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
$tmpl = '{{#with person as |foo|}}{{foo.first}} {{last}}{{/with}}';
$context = array(
        'person' => array(
            'first' => 'Alan',
            'last' => 'Johnson',
        ),
    );
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array();
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Alan Johnson