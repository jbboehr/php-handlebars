--TEST--
basic context - escaping expressions - 03
--DESCRIPTION--
basic context - escaping expressions - 03
escaping should properly handle amperstands
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
$tmpl = '{{awesome}}';
$context = array(
        'awesome' => 'Escaped, <b> looks like: &lt;b&gt;',
    );
$compileOptions = array();
$runtimeOptions = array();
$allOptions = array();
$vm = new VM();
echo $vm->render($tmpl, $context, $allOptions);
--EXPECT--
Escaped, &lt;b&gt; looks like: &amp;lt;b&amp;gt;