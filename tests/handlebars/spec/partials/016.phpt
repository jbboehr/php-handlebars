--TEST--
partials - rendering function partial in vm mode
--DESCRIPTION--
partials - rendering function partial in vm mode
Function partials output based in VM.
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
$tmpl = 'Dudes: {{#dudes}}{{> dude}}{{/dudes}}';
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
$partials = new DefaultRegistry(array(
        'dude' => function($context) {
      return $context['name'] . ' (' . $context['url'] . ') ';
    }/*function partial(context) {
      return context.name + ' (' + context.url + ') ';
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Dudes: Yehuda (http://yehuda) Alan (http://alan) 