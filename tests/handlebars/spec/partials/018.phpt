--TEST--
partials - Partials with slash paths
--DESCRIPTION--
partials - Partials with slash paths
Partials can use literal paths
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
$tmpl = 'Dudes: {{> shared/dude}}';
$context = array(
        'name' => 'Jeepers',
        'anotherDude' => 'Creepers',
    );
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Dudes: Jeepers