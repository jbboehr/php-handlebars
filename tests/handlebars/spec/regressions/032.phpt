--TEST--
Regressions - should allow hash with protected array names
--DESCRIPTION--
Regressions - should allow hash with protected array names
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
$tmpl = '{{helpa length="foo"}}';
$context = array(
        'array' => array(
            1,
        ),
        'name' => 'John',
    );
$helpers = new DefaultRegistry(array(
        'helpa' => function($options) {
        return $options['hash']['length'];
      }/*function(options) {
        return options.hash.length;
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
foo