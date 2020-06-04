--TEST--
helpers - String literal parameters - simple literals work
--DESCRIPTION--
helpers - String literal parameters - simple literals work
template with a simple String literal
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
$tmpl = 'Message: {{hello "world" 12 true false}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'hello' => function($param, $times, $bool1, $bool2) {
    if(!is_numeric($times)) { $times = "NaN"; }
    if(!is_bool($bool1)) {
        $bool1 = "NaB";
    } else {
        $bool1 = ($bool1 ? 'true' : 'false');
    }
    if(!is_bool($bool2)) {
        $bool2 = "NaB";
    } else {
        $bool2 = ($bool2 ? 'true' : 'false');
    }

    return "Hello $param $times times: $bool1 $bool2";
}/*function (param, times, bool1, bool2) {
        if (typeof times !== 'number') { times = 'NaN'; }
        if (typeof bool1 !== 'boolean') { bool1 = 'NaB'; }
        if (typeof bool2 !== 'boolean') { bool2 = 'NaB'; }
        return 'Hello ' + param + ' ' + times + ' times: ' + bool1 + ' ' + bool2;
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Message: Hello world 12 times: true false