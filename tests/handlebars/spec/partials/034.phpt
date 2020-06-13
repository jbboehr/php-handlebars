--TEST--
partials - partial blocks - should be able to access the @data frame from a partial-block
--DESCRIPTION--
partials - partial blocks - should be able to access the @data frame from a partial-block
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
$tmpl = '{{#> dude}}in-block: {{@root/value}}{{/dude}}';
$context = array(
        'value' => 'success',
    );
$partials = new DefaultRegistry(array(
        'shared/dude' => '{{name}}',
        'globalTest' => '{{anotherDude}}',
        'dude' => '<code>before-block: {{@root/value}} {{>   @partial-block }}</code>',
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<code>before-block: success in-block: success</code>