--TEST--
basic context - paths with hyphens - 01
--DESCRIPTION--
basic context - paths with hyphens - 01
Paths can contain hyphens (-)
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
$tmpl = '{{foo.foo-bar}}';
$context = array(
        'foo' => array(
            'foo-bar' => 'baz',
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
baz