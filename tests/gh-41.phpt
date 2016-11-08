--TEST--
GH #41 - Invalid helper type
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();
$vm->setHelpers(new Handlebars\DefaultRegistry(array(
    'nonexist' => true,
)));
try {
    $vm->render('{{nonexist "foo"}}');
} catch( Exception $e ) {
    echo $e->getMessage();
}
--EXPECT--
Unable to call value of type 1 on line 0, column 0
