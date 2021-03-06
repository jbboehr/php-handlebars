--TEST--
helpers - helperMissing - if a context is not found, custom helperMissing is used
--DESCRIPTION--
helpers - helperMissing - if a context is not found, custom helperMissing is used
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
$tmpl = '{{hello}} {{link_to world}}';
$context = array(
        'hello' => 'Hello',
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'world' => function() { return 'world!'; }/*function () { return 'world!'; }*/,
        'testHelper' => function() { return 'found it!'; }/*function () { return 'found it!'; }*/,
        'helperMissing' => function($mesg, $options = null) {
          if($options && $options['name'] === "link_to") {
            return new SafeString("<a>" . $mesg . "</a>");
          }
        }/*function (mesg, options) {
          if (options.name === 'link_to') {
            return new Handlebars.SafeString('<a>' + mesg + '</a>');
          }
        }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Hello <a>world</a>