--TEST--
builtin helpers - #each - each with @last
--DESCRIPTION--
builtin helpers - #each - each with @last
The @last variable is used
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
$tmpl = '{{#each goodbyes}}{{#if @last}}{{text}}! {{/if}}{{/each}}cruel {{world}}!';
$context = array(
        'goodbyes' => array(
            array(
                'text' => 'goodbye',
            ),
            array(
                'text' => 'Goodbye',
            ),
            array(
                'text' => 'GOODBYE',
            ),
        ),
        'world' => 'world',
    );
$helpers = new DefaultRegistry(array(
        'detectDataInsideEach' => function($options) { return isset($options['data']['exclaim'])? $options['data']['exclaim'] : ''; }/*function(options) { return options.data && options.data.exclaim; }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
GOODBYE! cruel world!