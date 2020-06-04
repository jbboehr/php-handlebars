--TEST--
builtin helpers - #with - with provides block parameter
--DESCRIPTION--
builtin helpers - #with - with provides block parameter
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
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Alan Johnson