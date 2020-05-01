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

var_dump($vm->render('{{#each a}}{{@key}}={{.}},{{/each}}', [
    'a' => (object) [
        'b' => 'c',
        'd' => 'e',
    ],
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
