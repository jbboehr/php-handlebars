--TEST--
Regressions - bug reported by @fat where lambdas weren't being properly resolved
--DESCRIPTION--
Regressions - bug reported by @fat where lambdas weren't being properly resolved
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
$tmpl = '<strong>This is a slightly more complicated {{thing}}.</strong>.
{{! Just ignore this business. }}
Check this out:
{{#hasThings}}
<ul>
{{#things}}
<li class={{className}}>{{word}}</li>
{{/things}}</ul>.
{{/hasThings}}
{{^hasThings}}

<small>Nothing to check out...</small>
{{/hasThings}}';
$context = array(
        'thing' => function() {
        return "blah";
      }/*function () {
        return 'blah';
      }*/,
        'things' => array(
            array(
                'className' => 'one',
                'word' => '@fat',
            ),
            array(
                'className' => 'two',
                'word' => '@dhg',
            ),
            array(
                'className' => 'three',
                'word' => '@sayrer',
            ),
        ),
        'hasThings' => function() {
        return true;
      }/*function () {
        return true;
      }*/,
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<strong>This is a slightly more complicated blah.</strong>.
Check this out:
<ul>
<li class=one>@fat</li>
<li class=two>@dhg</li>
<li class=three>@sayrer</li>
</ul>.
