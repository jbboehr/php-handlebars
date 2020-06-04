--TEST--
Post-Partial Behavior - Delimiters set in a partial should not affect the parent template.
--DESCRIPTION--
Post-Partial Behavior - Delimiters set in a partial should not affect the parent template.
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
$tmpl = '[ {{>include}} ]
[ .{{value}}.  .|value|. ]
';
$context = array(
        'value' => 'yes',
    );
$partials = new DefaultRegistry(array(
        'include' => '.{{value}}. {{= | | =}} .|value|.',
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
[ .yes.  .yes. ]
[ .yes.  .|value|. ]
