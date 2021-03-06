--TEST--
partials - dynamic partials - 01
--DESCRIPTION--
partials - dynamic partials - 01
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
        return 'dude';
      }/*function () {
        return 'dude';
      }*/,
    ));
$partials = new DefaultRegistry(array(
        'dude' => '{{name}} ({{url}}) ',
    ));
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array(
        'data' => false,
    );
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
$vm->setHelpers($helpers);
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Dudes: Yehuda (http://yehuda) Alan (http://alan) 