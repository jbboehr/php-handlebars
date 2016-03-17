--TEST--
Handlebars\VM::render() - helper throw
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\VM;
$vm = new VM();
$helpers = new DefaultRegistry(array(
    'badHelper' => function() { throw new Exception('whoops'); },
));
$vm->setHelpers($helpers);
try {
    var_dump($vm->render('{{badHelper}}'));
} catch( Exception $e ) {
    var_dump($e->getMessage());
}
--EXPECT--
string(6) "whoops"