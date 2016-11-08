--TEST--
GH #40 - Callable object
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\VM;
$vm = new VM();
class TestHelper {
    public function __invoke() {
        return true;
    }
}
var_dump($vm->render('{{testHelper}}', array(
    'testHelper' => new TestHelper(),
)));
--EXPECT--
string(4) "true"
