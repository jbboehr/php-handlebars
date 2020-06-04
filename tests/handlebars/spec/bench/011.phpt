--TEST--
Benchmarks - partial
--DESCRIPTION--
Benchmarks - partial
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
$tmpl = '{{#each peeps}}{{>variables}}{{/each}}';
$context = array(
        'peeps' => array(
            array(
                'name' => 'Moe',
                'count' => 15,
            ),
            array(
                'name' => 'Larry',
                'count' => 5,
            ),
            array(
                'name' => 'Curly',
                'count' => 1,
            ),
        ),
    );
$partials = new DefaultRegistry(array(
        'variables' => 'Hello {{name}}! You have {{count}} new messages.',
    ));
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array();
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Hello Moe! You have 15 new messages.Hello Larry! You have 5 new messages.Hello Curly! You have 1 new messages.