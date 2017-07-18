--TEST--
method lookup (GH-34)
--SKIPIF--
<?php if( !extension_loaded('handlebars') || version_compare(PHP_VERSION, "7", "<") ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\VM;
class Foo {
    private $baz = 'bat';
    public function bar() {
        return $this->baz;
    }
    private function privateBar() {
        return 'private' . $this->baz;
    }
}

$vm = new VM();
$ret = $vm->render('{{foo.bar}}', array(
    'foo' => new Foo(),
));
var_dump($ret);

$vm = new VM();
$ret = $vm->render('{{foo.privateBar}}', array(
    'foo' => new Foo(),
));
var_dump($ret);
--EXPECT--
string(3) "bat"
string(0) ""
