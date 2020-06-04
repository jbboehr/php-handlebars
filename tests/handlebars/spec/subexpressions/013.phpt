--TEST--
subexpressions - multiple subexpressions in a hash
--DESCRIPTION--
subexpressions - multiple subexpressions in a hash
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
$tmpl = '{{input aria-label=(t "Name") placeholder=(t "Example User")}}';
$context = array();
$helpers = new DefaultRegistry(array(
        'input' => function($options) {
        $hash        = $options['hash'];
        $ariaLabel   = (string) ($hash['aria-label'] instanceof SafeString ? $hash['aria-label'] : htmlspecialchars($hash['aria-label']));
        $placeholder = (string) ($hash['placeholder'] instanceof SafeString ? $hash['placeholder'] : htmlspecialchars($hash['placeholder']));
        return new SafeString('<input aria-label="' . $ariaLabel . '" placeholder="' . $placeholder . '" />');
      }/*function (options) {
        var hash = options.hash;
        var ariaLabel = Handlebars.Utils.escapeExpression(hash['aria-label']);
        var placeholder = Handlebars.Utils.escapeExpression(hash.placeholder);
        return new Handlebars.SafeString('<input aria-label="' + ariaLabel + '" placeholder="' + placeholder + '" />');
      }*/,
        't' => function($defaultString) {
        return new SafeString($defaultString);
      }/*function (defaultString) {
        return new Handlebars.SafeString(defaultString);
      }*/,
    ));
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
$vm->setHelpers($helpers);
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
<input aria-label="Name" placeholder="Example User" />