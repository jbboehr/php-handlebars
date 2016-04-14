--TEST--
Handlebars\BaseImpl::getHelpers()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\BaseImpl;
class FakeImpl extends BaseImpl {
    public function render($tmpl, $context = null, array $options = null) {}
    public function renderFile($filename, $context = null, array $options = null) {}
}
$reg = new DefaultRegistry();
$impl = new FakeImpl();
$impl->setHelpers($reg);
var_dump($impl->getHelpers() === $reg);
--EXPECT--
bool(true)
