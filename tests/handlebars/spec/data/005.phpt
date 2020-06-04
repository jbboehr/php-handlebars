--TEST--
data - hash values can be looked up via @foo
--DESCRIPTION--
data - hash values can be looked up via @foo
@foo as a parameter retrieves template data
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
$tmpl = '{{hello noun=@world}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'hello' => function($options) { return "Hello " . $options['hash']['noun']; }/*function (options) {
        return 'Hello ' + options.hash.noun;
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array(
        'data' => array(
            'world' => 'world',
        ),
    );
$allOptions = array(
        'data' => array(
            'world' => 'world',
        ),
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Hello world