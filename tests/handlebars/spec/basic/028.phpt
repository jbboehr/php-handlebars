--TEST--
basic context - should handle undefined and null
--DESCRIPTION--
basic context - should handle undefined and null
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
$tmpl = '{{awesome undefined null}}';
$context = array(
        'awesome' => function ($_undefined, $_null, $options) {
            return ($_undefined === null ? 'true' : 'false') . ' ' . ($_null === null ? 'true' : 'false') . ' ' . gettype($options);
          }/*function (_undefined, _null, options) {
            return (_undefined === undefined) + ' ' + (_null === null) + ' ' + (typeof options);
          }*/,
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
true true object