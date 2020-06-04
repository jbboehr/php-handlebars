--TEST--
string params mode - when inside a block in String mode, .. passes the appropriate context in the options hash
--DESCRIPTION--
string params mode - when inside a block in String mode, .. passes the appropriate context in the options hash
Proper context variable output
--SKIPIF--
<?php
if( true ) die('skip string params are not supported by the VM');
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
$tmpl = '{{#with dale}}{{tomdale ../need dad.joke}}{{/with}}';
$context = array(
        'dale' => array(),
        'need' => 'need-a',
    );
$helpers = new DefaultRegistry(array(
        'tomdale' => function($desire, $noun, $options) {
        return "STOP ME FROM READING HACKER NEWS I " .
                $options['contexts'][0][$desire] . " " . $noun;
      }/*function (desire, noun, options) {
        return 'STOP ME FROM READING HACKER NEWS I ' +
                options.contexts[0][desire] + ' ' + noun;
      }*/,
        'with' => function($context, $options) {
        return $options->fn($options['contexts'][0][$context]);
      }/*function (context, options) {
        return options.fn(options.contexts[0][context]);
      }*/,
    ));
$compileOptions = array(
        'stringParams' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'stringParams' => true,
    );
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
STOP ME FROM READING HACKER NEWS I need-a dad.joke