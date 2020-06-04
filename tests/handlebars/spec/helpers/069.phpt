--TEST--
helpers - name field - should include full id if a hash is passed
--DESCRIPTION--
helpers - name field - should include full id if a hash is passed
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
$tmpl = '{{#foo.helper bar=baz}}{{/foo.helper}}';
$context = array(
        'foo' => array(),
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'blockHelperMissing' => function() {
        $arguments = func_get_args();
        return 'missing: ' . $arguments[count($arguments)-1]['name'];
      }/*function () {
        return 'missing: ' + arguments[arguments.length - 1].name;
      }*/,
        'helperMissing' => function() {
        $arguments = func_get_args();
        return 'helper missing: ' . $arguments[count($arguments)-1]['name'];
      }/*function () {
        return 'helper missing: ' + arguments[arguments.length - 1].name;
      }*/,
        'helper' => function() {
        $arguments = func_get_args();
        return 'ran: ' . $arguments[count($arguments)-1]['name'];
      }/*function () {
        return 'ran: ' + arguments[arguments.length - 1].name;
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
helper missing: foo.helper