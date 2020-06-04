--TEST--
blocks - compat mode - block with deep recursive pathed lookup
--DESCRIPTION--
blocks - compat mode - block with deep recursive pathed lookup
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
$tmpl = '{{#outer}}Goodbye {{#inner}}cruel {{omg.yes}}{{/inner}}{{/outer}}';
$context = array(
        'omg' => array(
            'yes' => 'OMG!',
        ),
        'outer' => array(
            array(
                'inner' => array(
                    array(
                        'yes' => 'no',
                        'text' => 'goodbye',
                    ),
                ),
            ),
        ),
    );
$compileOptions = array(
        'compat' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'compat' => true,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Goodbye cruel OMG!