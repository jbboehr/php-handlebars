--TEST--
partials - partials with parameters
--DESCRIPTION--
partials - partials with parameters
Basic partials output based on current context.
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
$tmpl = 'Dudes: {{#dudes}}{{> dude others=..}}{{/dudes}}';
$context = array(
        'foo' => 'bar',
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
        'dude' => '{{others.foo}}{{name}} ({{url}}) ',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Dudes: barYehuda (http://yehuda) barAlan (http://alan) 