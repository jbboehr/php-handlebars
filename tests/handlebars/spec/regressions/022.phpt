--TEST--
Regressions - GH-1021: Each empty string key
--DESCRIPTION--
Regressions - GH-1021: Each empty string key
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
$tmpl = '{{#each data}}Key: {{@key}}
{{/each}}';
$context = array(
        'data' => array(
            '' => 'foo',
            'name' => 'Chris',
            'value' => 10000,
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Key: 
Key: name
Key: value
