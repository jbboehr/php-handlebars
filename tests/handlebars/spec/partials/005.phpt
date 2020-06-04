--TEST--
partials - failing dynamic partials
--DESCRIPTION--
partials - failing dynamic partials
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
$tmpl = 'Dudes: {{#dudes}}{{> (partial)}}{{/dudes}}';
$context = array(
        'dudes' => array(
            array(
                'name' => 'Yehuda',
                'url' => 'http://yehuda',
            ),
            array(
                'name' => 'Alan',
                'url' => 'http://alan',
            ),
        ),
    );
$helpers = new DefaultRegistry(array(
        'partial' => function() {
        return 'missing';
      }/*function () {
        return 'missing';
      }*/,
    ));
$partials = new DefaultRegistry(array(
        'dude' => '{{name}} ({{url}}) ',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECTF--
%AUncaught%A