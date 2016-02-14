--TEST--
Handlebars\Options::fn()
--SKIPIF--
<?php if( !extension_loaded('handlebars') ) die('skip '); ?>
--FILE--
<?php
use Handlebars\Options;
$options = new Options(array(
    'fn' => function() { var_dump('ok'); return 'okay'; },
    'inverse' => function() { var_dump('ok'); return 'okay'; },
));
var_dump($options->fn());
var_dump($options->inverse());
$options = new Options();
try {
    var_dump($options->fn());
} catch( Exception $e ) {
    var_dump($e->getMessage());
}
try {
    var_dump($options->inverse());
} catch( Exception $e ) {
    var_dump($e->getMessage());
}
--EXPECT--
string(2) "ok"
string(4) "okay"
string(2) "ok"
string(4) "okay"
string(13) "fn is not set"
string(13) "fn is not set"

