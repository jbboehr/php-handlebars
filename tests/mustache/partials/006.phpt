--TEST--
Inline Indentation - Whitespace should be left untouched.
--DESCRIPTION--
Inline Indentation - Whitespace should be left untouched.
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
require __DIR__ . "/../../utils.inc";

use Handlebars\DefaultRegistry;
use Handlebars\SafeString;
$tmpl = '  {{data}}  {{> partial}}
';
$context = array(
        'data' => '|',
    );
$partials = new DefaultRegistry(array(
        'partial' => '>' . "\n" . '>',
    ));
$compileOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$runtimeOptions = array();
$allOptions = array(
        'compat' => true,
        'mustacheStyleLambdas' => true,
    );
$vm = new VM();
$vm->setPartials($partials);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
  |  >
>
