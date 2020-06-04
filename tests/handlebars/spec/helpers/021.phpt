--TEST--
helpers - block helper inverted sections
--DESCRIPTION--
helpers - block helper inverted sections
an inverse wrapper is passed in as a new context
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
$tmpl = '{{#list people}}{{name}}{{^}}<em>Nobody\'s here</em>{{/list}}';
$context = array(
        'people' => array(
            array(
                'name' => 'Alan',
            ),
            array(
                'name' => 'Yehuda',
            ),
        ),
    );
$helpers = new DefaultRegistry(array(
        'list' => function($context, $options) {
    $out = '';
    if (count($context) > 0) {
        $out .= '<ul>';
        foreach ($context as $row) {
            $out .= '<li>';
            $out .= $options->fn($row);
            $out .= '</li>';
        }
        $out .= '</ul>';
    } elseif (isset($options->inverse)) {
        $out .= '<p>';
        $out .= $options->inverse($options->scope);
        $out .= '</p>';
    }
    return $out;
}/*function list(context, options) {
      if (context.length > 0) {
        var out = '<ul>';
        for (var i = 0, j = context.length; i < j; i++) {
          out += '<li>';
          out += options.fn(context[i]);
          out += '</li>';
        }
        out += '</ul>';
        return out;
      } else {
        return '<p>' + options.inverse(this) + '</p>';
      }
    }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<ul><li>Alan</li><li>Yehuda</li></ul>