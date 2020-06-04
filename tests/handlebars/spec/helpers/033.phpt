--TEST--
helpers - decimal number literals work
--DESCRIPTION--
helpers - decimal number literals work
template with a negative integer literal
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
$tmpl = 'Message: {{hello -1.2 1.2}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'hello' => function($times, $times2) {
    if (!is_numeric($times)) {
        $times = 'NaN';
    }
    if (!is_numeric($times2)) {
        $times2 = "NaN";
    }
    return "Hello $times $times2 times";
}/*function (times, times2) {
      if (typeof times !== 'number') { times = 'NaN'; }
      if (typeof times2 !== 'number') { times2 = 'NaN'; }
      return 'Hello ' + times + ' ' + times2 + ' times';
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Message: Hello -1.2 1.2 times