--TEST--
helper with options union (PHP 8)
--SKIPIF--
<?php
if( !extension_loaded('handlebars') ) die('skip ');
if( PHP_VERSION_ID < 80000 ) die('skip ');
?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\VM;
$vm = new VM();
$helpers = new DefaultRegistry(array(
    'testHelperWithUnionType' => function(string $a, string|Handlebars\Options|null $b = null) {
        var_dump(func_num_args());
    },
    'testHelperWithUnionType2' => function(string $a, string|int|null $b = null) {
        var_dump(func_num_args());
    },
    'testHelperWithUnionType3' => function(string $a, stdClass|Handlebars\Options $b = null) {
        var_dump(func_num_args());
    },
));
$vm->setHelpers($helpers);
$vm->render('{{testHelperWithUnionType "foo"}}');
$vm->render('{{testHelperWithUnionType2 "foo"}}');
$vm->render('{{testHelperWithUnionType3 "foo"}}');
--EXPECT--
int(2)
int(1)
int(2)
