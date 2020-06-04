--TEST--
basic context - functions - 01
--DESCRIPTION--
basic context - functions - 01
functions are bound to the context
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
$tmpl = '{{awesome}}';
$context = array(
        'awesome' => function($options) { return $options->scope['more']; }/*function () { return this.more; }*/,
        'more' => 'More awesome',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
More awesome