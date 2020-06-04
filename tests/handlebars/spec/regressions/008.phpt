--TEST--
Regressions - GH-408: Multiple loops fail
--DESCRIPTION--
Regressions - GH-408: Multiple loops fail
It should output multiple times
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
$tmpl = '{{#.}}{{name}}{{/.}}{{#.}}{{name}}{{/.}}{{#.}}{{name}}{{/.}}';
$context = array(
        array(
            'name' => 'John Doe',
            'location' => array(
                'city' => 'Chicago',
            ),
        ),
        array(
            'name' => 'Jane Doe',
            'location' => array(
                'city' => 'New York',
            ),
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
John DoeJane DoeJohn DoeJane DoeJohn DoeJane Doe