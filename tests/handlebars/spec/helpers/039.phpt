--TEST--
helpers - negative number literals work - 01
--DESCRIPTION--
helpers - negative number literals work - 01
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
$tmpl = 'Message: {{hello -12}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'hello' => function($times) {
    if(!is_numeric($times)) {
        $times = "NaN";
    }
    return "Hello $times times";
}/*function (times) {
      if (typeof times !== 'number') { times = 'NaN'; }
      return 'Hello ' + times + ' times';
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Message: Hello -12 times