--TEST--
helpers - block helper should have context in this
--DESCRIPTION--
helpers - block helper should have context in this
--SKIPIF--
<?php
if( true ) die('skip problem with prop handlers');
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
$tmpl = '<ul>{{#people}}<li>{{#link}}{{name}}{{/link}}</li>{{/people}}</ul>';
$context = array(
        'people' => array(
            array(
                'name' => 'Alan',
                'id' => 1,
            ),
            array(
                'name' => 'Yehuda',
                'id' => 2,
            ),
        ),
    );
$helpers = new DefaultRegistry(array(
        'link' => function($options) {
    return '<a href="/people/'.$options->scope['id'].'">' . $options->fn($options->scope) . '</a>';
}/*function link(options) {
      return '<a href="/people/' + this.id + '">' + options.fn(this) + '</a>';
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<ul><li><a href="/people/1">Alan</a></li><li><a href="/people/2">Yehuda</a></li></ul>