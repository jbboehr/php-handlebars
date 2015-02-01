#!/usr/bin/env php
<?php

/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

// Utils

function handlebarsc($tmpl, $op) {
    $return_var = 1;
    $output = array();
    $command = 'echo ' . escapeshellarg($tmpl) . ' | handlebarsc ' . escapeshellarg($op) . ' 2>&1';
    exec($command, $output, $return_var);
    if( $return_var == 127 ) {
        echo "handlebarsc is not available for printer testing!\n";
        exit(1);
    }
    return array(($return_var == 0), join("\n", $output), $command);
}

function testFile($test, $specName, $number) {
    $safeName = strtolower(trim(preg_replace('/[^a-z0-9]+/i', '-', $test['it'] . '-' . $test['description']), '-'));
    $testFile = __DIR__ . '/tests/spec-handlebars-' . $specName . '-' . sprintf("%03d", $number) . '-' . $safeName . '.phpt';
    return $testFile;
}

function testHead($test, $specName, $number) {
    $output = '';
    $output .= '--TEST--' . "\n";
    $output .= $specName . ' #' . $number . ' - ' . $test['it'] . "\n";
    $output .= '--DESCRIPTION--' . "\n";
    $output .= $test['description'] . "\n";
    $output .= '--SKIPIF--' . "\n";
    $output .= "<?php if( !extension_loaded('handlebars') ) die('skip '); ?>" . "\n";
    $output .= '--FILE--' . "\n";
    $output .= '<?php' . "\n";
    //$output .= '$test = ' . var_export($test, true) . ';' . "\n";
    return $output;
}

function testBody($test, $specName) {
    switch( $specName ) {
        case 'parser':
            return testBodyParser($test);
            break;
        case 'tokenizer':
            return testBodyTokenizer($test);
            break;
        default:
            echo "Unknown spec: ", $specName, PHP_EOL;
            exit(1);
            break;
    }
}

function testBodyTokenizer($test) {
    // Get the printed tokens from handlebarsc
    list($ok, $printed) = handlebarsc($test['template'], 'lex');
    if( $ok != empty($test['exception']) ) {
        $msg = sprintf("handlebarsc error did not match test error: %d != %d, output was %s",
                $ok, empty($test['exception']), $printed);
        trigger_error($msg, E_USER_ERROR);
    }
    $printed = preg_replace('/[\r\n]+/', " ", $printed);
    
    $output .= '$tmpl = ' . var_export($test['template'], true) . ';' . PHP_EOL;
    $output .= 'var_export(handlebars_lex_print($tmpl));' . PHP_EOL;
    //$output .= 'var_export(handlebars_lex($test["template"]));' . PHP_EOL;
    $output .= '--EXPECT--' . PHP_EOL;
    $output .= var_export($printed, true);
    //$output .= var_export($test['expected'], true);
    return $output;
}

function testBodyParser($test) {
    // Get the printed ast from handlebarsc
    list($ok, $printed, $command) = handlebarsc($test['template'], 'parse');
    if( $ok != empty($test['exception']) ) {
        $msg = sprintf("handlebarsc error did not match test error: %d != %d, output was %s",
                $ok, empty($test['exception']), $printed);
        trigger_error($msg, E_USER_ERROR);
    }
    
    $output .= '/* ' . $command . ' */' . PHP_EOL;
    $output .= '$tmpl = ' . var_export($test['template'], true) . ';' . PHP_EOL;
    $output .= '$v = handlebars_parse_print($tmpl); $v ? var_export($v) : var_export(handlebars_error());' . PHP_EOL;
    //$output .= 'var_export(handlebars_parse($test["template"]));' . PHP_EOL;
    $output .= '--EXPECT--' . PHP_EOL;
    $output .= var_export($printed, true);
    //$output .= var_export($test['expected'], true);
    return $output;
}



// Main
$specDir = __DIR__ . '/spec/handlebars/spec';
$parserSpecFile = $specDir . '/parser.json';
$tokenizerSpecFile = $specDir. '/tokenizer.json';

foreach( array($tokenizerSpecFile, $parserSpecFile) as $file ) {
    $specName = substr(basename($file), 0, strpos(basename($file), '.'));
    $tests = json_decode(file_get_contents($file), true);
    $number = 0;
    foreach( $tests as $test ) {
        ++$number;
        $testFile = testFile($test, $specName, $number);
        $output = '';
        $output .= testHead($test, $specName, $number);
        $output .= testBody($test, $specName);
        file_put_contents($testFile, $output);
    }
}
