--TEST--
basic context - boolean
--DESCRIPTION--
basic context - boolean
booleans show the contents when true
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
$tmpl = '{{#goodbye}}GOODBYE {{/goodbye}}cruel {{world}}!';
$context = array(
        'goodbye' => true,
        'world' => 'world',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE cruel world!