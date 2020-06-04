--TEST--
partials - compat mode - partials inherit compat
--DESCRIPTION--
partials - compat mode - partials inherit compat
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
$tmpl = 'Dudes: {{> dude}}';
$context = array(
        'root' => 'yes',
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
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
        'dude' => '{{#dudes}}{{name}} ({{url}}) {{root}} {{/dudes}}',
    ));
$compileOptions = array(
        'compat' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'compat' => true,
    );
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Dudes: Yehuda (http://yehuda) yes Alan (http://alan) yes 