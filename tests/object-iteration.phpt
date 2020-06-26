--TEST--
Object iteration
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\VM;
$vm = new VM();

var_dump($vm->render('{{#each a}}{{@key}}={{.}},{{/each}}', [
    'a' => new stdClass(),
]));

// casting array to object is broken on ppc64le
$obj = new stdClass;
$obj->b = 'c';
$obj->d = 'e';
var_dump($vm->render('{{#each a}}{{@key}}={{.}},{{/each}}', [
    'a' => $obj,
]));

var_dump($vm->render('{{#each a}}{{@key}}={{.}},{{/each}}', [
    'a' => new ArrayObject([
        'b' => 'c',
        'd' => 'e',
    ]),
]));

--EXPECT--
string(0) ""
string(8) "b=c,d=e,"
string(8) "b=c,d=e,"
