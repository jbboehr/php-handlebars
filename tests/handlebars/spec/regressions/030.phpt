--TEST--
Regressions - GH-1319: "unless" breaks when "each" value equals "null"
--DESCRIPTION--
Regressions - GH-1319: "unless" breaks when "each" value equals "null"
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
$tmpl = '{{#each list}}{{#unless ./prop}}parent={{../value}} {{/unless}}{{/each}}';
$context = array(
        'value' => 'parent',
        'list' => array(
            NULL,
            'a',
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
parent=parent parent=parent 