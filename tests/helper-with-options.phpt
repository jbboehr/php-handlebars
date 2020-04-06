--TEST--
helper with options
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\DefaultRegistry;
use Handlebars\VM;
$vm = new VM();
class TestHelperNoTypesNonClosure {
    public function __invoke($a) {
        var_dump(func_num_args());
        var_dump($a);
    }
}
$helpers = new DefaultRegistry(array(
    'testHelperNoTypes' => function($a) {
        var_dump(func_num_args());
        var_dump($a);
        var_dump(gettype(func_get_arg(1)));
        var_dump(get_class(func_get_arg(1)));
    },
    'testHelperNoTypesNonClosure' => new TestHelperNoTypesNonClosure(),
    'testHelperWithOptionsType' => function($a, \Handlebars\Options $b) {
        var_dump(func_num_args());
        var_dump($a);
        var_dump(gettype($b));
        var_dump(get_class($b));
    },
    'testHelperWithInvalidArgType' => function(string $a, string $b = null) {
        var_dump(func_num_args());
        var_dump($a);
        var_dump(gettype($b));
    },
    'testHelperWithExtraArg' => function($a, $b) {
        var_dump(func_num_args());
        var_dump($a);
        var_dump(gettype($b));
        var_dump(get_class($b));
    },
));
$vm->setHelpers($helpers);
$vm->render('{{testHelperNoTypes "foo"}}');
$vm->render('{{testHelperNoTypesNonClosure "foo"}}');
$vm->render('{{testHelperWithOptionsType "foo"}}');
$vm->render('{{testHelperWithInvalidArgType "foo"}}');
$vm->render('{{testHelperWithExtraArg "foo"}}');
--EXPECT--
int(2)
string(3) "foo"
string(6) "object"
string(18) "Handlebars\Options"
int(1)
string(3) "foo"
int(2)
string(3) "foo"
string(6) "object"
string(18) "Handlebars\Options"
int(1)
string(3) "foo"
string(4) "NULL"
int(2)
string(3) "foo"
string(6) "object"
string(18) "Handlebars\Options"
