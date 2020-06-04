--TEST--
helpers - helper block with complex lookup expression
--DESCRIPTION--
helpers - helper block with complex lookup expression
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
$tmpl = '{{#goodbyes}}{{../name}}{{/goodbyes}}';
$context = array(
        'name' => 'Alan',
    );
$helpers = new DefaultRegistry(array(
        'goodbyes' => function($options) {
$byes = array('Goodbye', 'goodbye', 'GOODBYE');
$out = '';
foreach ($byes as $bye) {
$out .= $bye.' ';
$out .= $options->fn();
$out .= '! ';
}
return $out;
}/*function (options) {
      var out = '';
      var byes = ['Goodbye', 'goodbye', 'GOODBYE'];
      for (var i = 0, j = byes.length; i < j; i++) {
        out += byes[i] + ' ' + options.fn({}) + '! ';
      }
      return out;
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Goodbye Alan! goodbye Alan! GOODBYE Alan! 