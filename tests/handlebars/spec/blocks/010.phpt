--TEST--
blocks - block with deep nested complex lookup
--DESCRIPTION--
blocks - block with deep nested complex lookup
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
$tmpl = '{{#outer}}Goodbye {{#inner}}cruel {{../sibling}} {{../../omg}}{{/inner}}{{/outer}}';
$context = array(
        'omg' => 'OMG!',
        'outer' => array(
            array(
                'sibling' => 'sad',
                'inner' => array(
                    array(
                        'text' => 'goodbye',
                    ),
                ),
            ),
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Goodbye cruel sad OMG!