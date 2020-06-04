--TEST--
Benchmarks - complex
--DESCRIPTION--
Benchmarks - complex
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
$tmpl = '<h1>{{header}}</h1>
{{#if items}}
  <ul>
    {{#each items}}
      {{#if current}}
        <li><strong>{{name}}</strong></li>
      {{^}}
        <li><a href="{{url}}">{{name}}</a></li>
      {{/if}}
    {{/each}}
  </ul>
{{^}}
  <p>The list is empty.</p>
{{/if}}
';
$context = array(
        'header' => function () {
      return "Colors";
    }/*function () {
      return 'Colors';
    }*/,
        'hasItems' => true,
        'items' => array(
            array(
                'name' => 'red',
                'current' => true,
                'url' => '#Red',
            ),
            array(
                'name' => 'green',
                'current' => false,
                'url' => '#Green',
            ),
            array(
                'name' => 'blue',
                'current' => false,
                'url' => '#Blue',
            ),
        ),
    );
$compileOptions = array(
        'data' => false,
    );
$runtimeOptions = array();
$allOptions = array(
        'data' => false,
    );
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<h1>Colors</h1>
  <ul>
        <li><strong>red</strong></li>
        <li><a href="#Green">green</a></li>
        <li><a href="#Blue">blue</a></li>
  </ul>
