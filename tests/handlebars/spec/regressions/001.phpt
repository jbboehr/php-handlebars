--TEST--
Regressions - GH-94: Cannot read property of undefined
--DESCRIPTION--
Regressions - GH-94: Cannot read property of undefined
Renders without an undefined property error
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
$tmpl = '{{#books}}{{title}}{{author.name}}{{/books}}';
$context = array(
        'books' => array(
            array(
                'title' => 'The origin of species',
                'author' => array(
                    'name' => 'Charles Darwin',
                ),
            ),
            array(
                'title' => 'Lazarillo de Tormes',
            ),
        ),
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
The origin of speciesCharles DarwinLazarillo de Tormes