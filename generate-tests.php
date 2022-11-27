#!/usr/bin/env php
<?php

// generate the package.xml test entries with:
// find tests -type f -not -name .gitignore | sort | xargs -I{} echo '   <file name="{}" role="test" />'

require __DIR__ . '/tests/utils.inc';

if( !extension_loaded('psr') ) {
    eval("namespace Psr\Log;\ninterface LoggerInterface {}\ninterface LoggerAwareInterface {}");
}
if( !extension_loaded('handlebars') ) {
    require 'handlebars.stub.php';
    require 'handlebars-ast.stub.php';
}

$startTime = microtime(true);
$nTests = 0;
$handlebarsSpecDir = isset($_ENV['HANDLEBARS_SPEC_DIR']) ? $_ENV['HANDLEBARS_SPEC_DIR'] : __DIR__ . '/spec/handlebars';
$mustacheSpecDir = isset($_ENV['MUSTACHE_SPEC_DIR']) ? $_ENV['MUSTACHE_SPEC_DIR'] : __DIR__ . '/spec/mustache';

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

function isIntArray($array)
{
    if( !is_array($array) ) {
        return false;
    }

    $i = 0;
    foreach( $array as $k => $v ) {
        if( is_string($k) ) {
            return false;
        } else if( $k !== $i++ ) {
            return false;
        }
    }

    return true;
}

function indent($n, $str = '')
{
    return str_pad($str, $n * 4, ' ', STR_PAD_LEFT);
}

function indentVarExport($n, $var)
{
    return str_replace("\n", "\n" . indent($n), varExport($var));
}

class ClosureHolder
{
    private $closureText;

    public function __construct($closureText)
    {
        $this->closureText = $closureText;
    }

    public function __toString()
    {
        return $this->closureText;
    }
}

function convertLambdas(&$data)
{
    if( !is_array($data) ) {
        return;
    }
    foreach( $data as $k => $v ) {
        if( !is_array($v) ) {
            continue;
        }
        $unimpl = 'function () { throw new Exception("unimplemented test fixture"); }';
        if( !empty($v['!code']) ) {
            $js = $v['javascript'] ?? null;
            $php = $v['php'] ?? 'function () { throw new Exception("unimplemented test fixture"); }';
            $data[$k] = new ClosureHolder($php . '/*' . $js . '*/');
        } else if (!empty($v['__tag__']) && $v['__tag__'] === 'code') {
            $js = $v['js'] ?? null;
            $php = $v['php'] ?? $unimpl;
            $data[$k] = new ClosureHolder('function($text) {' . $php . '}' . '/*' . $js . '*/');
        } else {
            convertLambdas($data[$k]);
        }
    }
}

function varExport($var, $indent = 0)
{
    convertLambdas($var);

    if( $var instanceof ClosureHolder ) {
        return (string) $var;
    } else if( is_array($var) ) {
        if( empty($var) ) {
            return 'array()';
        } else {
            $output = "array(\n";
            $isNormalArray = isIntArray($var);
            foreach( $var as $k => $v ) {
                if( $k === '!sparsearray' ) {
                    continue;
                }
                $output .= indent($indent + 1)
                        . (!$isNormalArray ? var_export($k, true)
                        . ' => ' : '' )
                        . varExport($v, $indent + 1) . ",\n";
            }
            $output .= indent($indent) . ')';
            return $output;
        }
    } else {
        $v = var_export($var, true);
        if( is_string($var) ) {
            $v = str_replace("\n", $v[0] . ' . "\n" . ' . $v[0], $v);
            $v = str_replace("\r", $v[0] . ' . "\r" . ' . $v[0], $v);
        }
        return $v;
    }
}

function patch_tokens(array &$tokens) {
    foreach( $tokens as $k => $token ) {
        $tokens[$k] = new Handlebars\Token($token['name'], $token['text']);
    }
    return $tokens;
}

function patch_opcode(array $opcode) {
    // @todo we could fix this by adding a distinct null operand type
    if( $opcode['opcode'] === 'emptyHash' ) {
        // Add null operand - currently only supports fixed number of operands
        if( count($opcode['args']) === 0 ) {
            $opcode['args'] = array(null);
        }
    } else if( $opcode['opcode'] === 'pushId' ) {
        // Add null operand - currently only supports fixed number of operands
        if( count($opcode['args']) === 2 ) {
            $opcode['args'][] = null;
        }
        // Stringify - array operands only support strings
        if( is_array($opcode['args'][1]) ) {
            $opcode['args'][1][0] = (string) $opcode['args'][1][0];
            $opcode['args'][1][1] = (string) $opcode['args'][1][1];
        }
    } else if( $opcode['opcode'] === 'lookupBlockParam' ) {
        // Stringify - array operands only support strings
        if( is_array($opcode['args'][0]) ) {
            settype($opcode['args'][0][0], 'string');
            settype($opcode['args'][0][1], 'string');
        }
    } else if( $opcode['opcode'] === 'pushLiteral' ) {
        // Stringify - operands don't support floats/decimals
        if( is_float($opcode['args'][0]) ) {
            settype($opcode['args'][0], 'string');
        }
    }
    unset($opcode['loc']);
    return new Handlebars\Opcode($opcode['opcode'], $opcode['args']);
}

function patch_opcodes(array $opcodes) {
    foreach( $opcodes as $k => $opcode ) {
        $opcodes[$k] = patch_opcode($opcode);
    }
    return $opcodes;
}

function patch_context(array $context) {
    $opcodes = patch_opcodes($context['opcodes']);
    $children = array();
    foreach( $context['children'] as $k => $v ) {
        $children[$k] = patch_context($v);
    }
    $blockParams = isset($context['blockParams']) ? $context['blockParams'] : null;

    $obj = new Handlebars\Program($opcodes, $children, $blockParams);

    foreach( array('useDepths', 'usePartial', 'useDecorators') as $k ) {
        if( !empty($context[$k]) ) {
            $obj->$k = true;
        }
    }
    foreach( array('stringParams', 'trackIds') as $k ) {
            $obj->$k = !empty($context[$k]);
    }

    return $obj;
}

function token_print($tokens) {
    $str = '';
    foreach( $tokens as $token ) {
        $str .= sprintf('%s [%s] ', $token['name'], addcslashes($token['text'], "\t\r\n"));
    }
    return rtrim($str, ' ');
}

function hbs_test_file(array $test) {
    //$name = $test['it'] . '-' . $test['description'];
    //$safeName = strtolower(trim(preg_replace('/[^a-z0-9]+/i', '-', $name), '-'));
    if( $test['suiteType'] === 'mustache' ) {
        $st = 'mustache';
    } else {
        $st = 'handlebars/' . $test['suiteType'];
    }
    $testFile = __DIR__ . '/tests'
        . '/' . $st
        . '/' . $test['suiteName']
        . '/' . sprintf("%03d", $test['i']) /*. '-' . $safeName */ . '.phpt';
    return $testFile;
}

function hbs_generate_test_head(array &$test) {
    // Skip this test for now
    $skip = "!extension_loaded('handlebars')";
    $reason = '';
    $utilsPrefix = $test['suiteType'] === 'mustache' ? '' : '../';
    $testName = $test['description'] . ' - ' . $test['it'];
    if (!empty($test['number'])) {
        $testName .= ' - ' . $test['number'];
    }
    $extraSkip = '';

    if( $test['suiteType'] == 'export' || $test['suiteName'] == 'parser' || $test['suiteName'] == 'tokenizer' ) {
        $extraSkip .= <<<EOF
if( !class_exists('Handlebars\\Compiler') ) die('skip handlebars AST not enabled');
EOF;
    }

    switch( $test['description'] . ' - ' . $test['it'] ) {
    	case 'Standalone Indentation - Each line of the partial should be indented before rendering.':
            $skip = 'true';
            $reason = 'skip for now';
            break;
    }

    if( $test['suiteType'] == 'export' ) {
        switch ($testName) {
            case 'basic context - escaping - 01':
            case 'Tokenizer - supports escaping delimiters':
            case 'Tokenizer - supports escaping multiple delimiters':
            case 'Tokenizer - supports escaping a triple stash':
            case 'Tokenizer - supports escaped mustaches after escaped escape characters':
            case 'Tokenizer - supports escaped escape characters after escaped mustaches':
                $skip = 'true';
                $reason = 'this test is correct, but handlebars.js does not join two adjacent content blocks';
                break;
            case 'helpers - helper for nested raw block gets raw content':
            case 'helpers - raw block parsing (with identity helper-function) - helper for nested raw block gets raw content':
                $skip = 'true';
                $reason = 'skip for now';
                break;
        }
    }

    if( $test['suiteType'] == 'spec' ) {
        if( $test['suiteName'] == 'string-params' ) {
            $skip = 'true';
            $reason = 'string params are not supported by the VM';
        } else if( $test['suiteName'] == 'track-ids' ) {
            $skip = 'true';
            $reason = 'track ids are not supported by the VM';
        } else if( $test['description'] == 'partials - inline partials' ) {
            $skip = 'true';
            $reason = 'inline partials are not supported by the VM';
        } else if( $test['description'] == 'blocks - decorators' ) {
            $skip = 'true';
            $reason = 'decorators are not supported by the VM';
        }

        switch ($testName) {
            case 'helpers - block helper should have context in this':
                $skip = 'true';
                $reason = 'problem with prop handlers';
                break;
            case 'helpers - block params - should take presednece over parent block params';
                $skip = 'true';
                $reason = 'libhandlebars currently does not expose options.blockParams';
                break;
            case 'Regressions - should support multiple levels of inline partials':
            case 'Regressions - GH-1089: should support failover content in multiple levels of inline partials':
            case 'Regressions - GH-1099: should support greater than 3 nested levels of inline partials':
                $skip = 'true';
                $reason = 'inline partials are not supported by the VM';
                break;
            case 'Regressions - GH-1186: Support block params for existing programs':
                $skip = 'true';
                $reason = 'decorators are not supported by the VM';
                break;
            case 'Regressions - GH-1135 : Context handling within each iteration':
                $skip = 'true';
                $reason = 'need to implement deep equal for this?';
                break;
            case 'subexpressions - in string params mode,':
            case 'subexpressions - as hashes in string params mode':
                $skip = 'true';
                $reason = 'string params are not supported by the VM';
                break;
            case "subexpressions - subexpressions can't just be property lookups":
                $skip = 'true';
                $reason = 'skip for now';
                break;
        }
    }

    if ($test['suiteType'] === 'mustache' && $test['suiteName'] === 'lambdas') {
        $extraSkip .= <<<EOF
if( !defined('Handlebars\\Compiler::MUSTACHE_STYLE_LAMBDAS') ) die('skip configured libhandlebars version has no lambda support');
EOF;
    }

    return join("\n", array_filter([
        '--TEST--',
        $testName,
        '--DESCRIPTION--',
        $testName,
        $test['message'] ?? null,
        '--SKIPIF--',
        "<?php",
        "if( $skip ) die('skip $reason');",
        $extraSkip,
        "?>",
        '--FILE--',
        '<?php',
        'use Handlebars\Compiler;',
        'use Handlebars\Parser;',
        'use Handlebars\Tokenizer;',
        'use Handlebars\Utils;',
        'use Handlebars\VM;',
        'require __DIR__ . "/' . $utilsPrefix . '../../utils.inc";',
    ])) . "\n";
}

function hbs_write_file($file, $contents) {
    // Make sure dir exists
    $dir = dirname($file);
    if( !is_dir($dir) ) {
        mkdir($dir, 0777, true);
    }

    // Write
    return file_put_contents($file, $contents);
}



// Test genenerator main

function hbs_generate_export_test_body(array $test) {

    $compileOptions = $test['compileOptions'] ?? [];

    $expectedOpcodes = patch_context($test['opcodes']);

    return join("\n", array_filter([
        '$compileOptions = ' . var_export($compileOptions, true) . ';',
        '$tmpl = ' . var_export($test['template'], true) . ';',
        'myprint(Compiler::compile($tmpl, $compileOptions), true);',
        'myprint(gettype(Compiler::compilePrint($tmpl, $compileOptions)), true);',
        '--EXPECT--',
        myprint($expectedOpcodes) . myprint('string'),
    ]));
}

function hbs_generate_spec_test_body_generic(array $test) {
    $runtimeOptions = $test['runtimeOptions'] ?? [];
    $compileOptions = $test['compileOptions'] ?? [];
    $allOptions = array_merge($compileOptions, $runtimeOptions);
    $helpers = (array) @$test['helpers'];
    $partials = (array) @$test['partials'];
    $context = isset($test['data']) ? $test['data'] : null;
    $expectHead = null;
    $message = null;
    if( array_key_exists('expected', $test) ) {
        $expected = $test['expected'];
    } else if( !empty($test['exception']) ) {
        $expectHead = 'EXPECTF';
        // @todo improve this
        $expected = '%AUncaught%A';
        if( !empty($test['message']) ) {
            /*$expected =*/ $message = $test['message'];
        } else {
            //$expected = 'exception';
        }
    } else {
        echo "Whoops\n";
        exit(1);
    }

    return PHP_EOL . join(PHP_EOL, array_filter([
        'use Handlebars\DefaultRegistry;',
        'use Handlebars\SafeString;',
        '$tmpl = ' . var_export($test['template'], true) . ';',
        '$context = ' . varExport($context, true) . ';',
        $helpers ? '$helpers = new DefaultRegistry(' . varExport($helpers, true) . ');' : null,
        $partials ? '$partials = new DefaultRegistry(' . varExport($partials, true) . ');' : null,
        '$compileOptions = ' . varExport($compileOptions, true) . ';',
        '$runtimeOptions = ' . varExport($runtimeOptions, true) . ';',
        '$allOptions = ' . varExport($allOptions, true) . ';',
        //'$knownHelpers = ' . var_export($knownHelpers, true) . ';',
        '$vm = new VM();',
        $helpers ? '$vm->setHelpers($helpers);' : null,
        $partials ? '$vm->setPartials($partials);' : null,
        'echo $vm->render($tmpl, $context, $allOptions);',
        $message ? '/* Error message: ' . addcslashes($message, "\r\n") . ' */' : null,
        '--'. ($expectHead ?: 'EXPECT') . '--',
        $expected
    ]));
}

function hbs_generate_spec_test_body_tokenizer(array $test) {
    $output = '';
    $output .= '$tmpl = ' . var_export($test['template'], true) . ';' . PHP_EOL;
    $output .= 'myprint(Tokenizer::lexPrint($tmpl), true);' . PHP_EOL;
    $output .= 'echo PHP_EOL;' . PHP_EOL;
    $output .= 'myprint(Tokenizer::lex($tmpl), true);' . PHP_EOL;
    $output .= '--EXPECT--' . PHP_EOL;
    $output .= myprint(token_print($test['expected']));
    $output .= PHP_EOL;
    $output .= myprint(patch_tokens($test['expected']));
    return $output;
}

function hbs_generate_spec_test_body_parser(array $test) {
    if( empty($test['exception']) ) {
        $expected = rtrim($test['expected'], " \t\r\n");
    } else {
        $expected = false; //$test['message'];
    }

    $output = '';
    $output .= '$tmpl = ' . var_export($test['template'], true) . ';' . PHP_EOL;
    $output .= '
try {
    var_export(Parser::parsePrint($tmpl));
    var_export(gettype(Parser::parse($tmpl)));
} catch( Handlebars\ParseException $e ) {
    echo "exception: ", $e->getMessage();
}
' . PHP_EOL;
    $output .= 'echo PHP_EOL;' . PHP_EOL;
    $output .= '' . PHP_EOL;
    if( empty($test['exception']) ) {
        $output .= '--EXPECT--' . PHP_EOL;
        $output .= var_export($expected, true);
        $output .= var_export('array', true);
    } else {
        $output .= '--EXPECTF--' . PHP_EOL;
        $output .= 'exception%s';
        //$output .= $test['message'];
    }
    return $output;
}

function hbs_generate_spec_test_body(array $test) {
    switch( $test['suiteName'] ) {
        case 'parser':
            return hbs_generate_spec_test_body_parser($test);
            break;
        case 'tokenizer':
            return hbs_generate_spec_test_body_tokenizer($test);
            break;
        default:
            return hbs_generate_spec_test_body_generic($test);
            break;
    }
}

function hbs_generate_export_test(array $test) {
    $file = hbs_test_file($test);
    $body = hbs_generate_export_test_body($test);
    if( !$body ) {
        return;
    }

    $output = '';
    $output .= hbs_generate_test_head($test);
    $output .= $body;
    hbs_write_file($file, $output);
}

function hbs_generate_spec_test(array $test) {
    $file = hbs_test_file($test);
    $head = hbs_generate_test_head($test);
    $body = hbs_generate_spec_test_body($test);
    if( !$body ) {
        return;
    }

    $output = '';
    $output .= $head; //hbs_generate_test_head($test);
    $output .= $body;
    hbs_write_file($file, $output);
}

function hbs_generate_mustache_spec_test(array $test) {
    // Convert format to handlebars
    $test['description'] = $test['name'];
    $test['it'] = $test['desc'];
    $test['compileOptions'] = array('compat' => true, 'mustacheStyleLambdas' => true);

    $file = hbs_test_file($test);
    $head = hbs_generate_test_head($test);
    $body = hbs_generate_spec_test_body($test);
    if( !$body ) {
        return;
    }

    $output = '';
    $output .= $head; //hbs_generate_test_head($test);
    $output .= $body;
    hbs_write_file($file, $output);
}



// Main

// Handlebars Spec
$specDir = $handlebarsSpecDir . '/spec/';
foreach( scandir($specDir) as $file ) {
    if( $file[0] === '.' || substr($file, -5) !== '.json' ) {
        continue;
    }
    $filePath = $specDir . $file;
    $suiteName = substr(basename($filePath), 0, strpos(basename($filePath), '.'));
    $tests = json_decode(file_get_contents($filePath), true);
    $i = 0;
    foreach( $tests as $test ) {
        ++$i;
        $test['suiteType'] = 'spec';
        $test['suiteName'] = $suiteName;
        $test['i'] = $i;
        hbs_generate_spec_test($test);
    }
}

// Handlebars Export
$exportDir = $handlebarsSpecDir . '/export/';
foreach( scandir($exportDir) as $file ) {
    if( $file[0] === '.' || substr($file, -5) !== '.json' ) {
        continue;
    }
    $filePath = $exportDir . $file;
    $fileName = basename($filePath);
    $suiteName = substr($fileName, 0, -strlen('.json'));
    $tests = json_decode(file_get_contents($filePath), true);

    if( !$tests ) {
        trigger_error("No tests in file: " . $file, E_USER_WARNING);
        continue;
    }

    $i = 0;
    foreach( $tests as $test ) {
        ++$i;
        $test['suiteType'] = 'export';
        $test['suiteName'] = $suiteName;
        $test['i'] = $i;
        hbs_generate_export_test($test);
    }
}

// Mustache Spec
$mustacheSpecDir = $mustacheSpecDir . '/specs/';
foreach( scandir($mustacheSpecDir) as $file ) {
    if( $file[0] === '.' || /*$file[0] === '~' ||*/ substr($file, -5) !== '.json' ) {
        continue;
    }
    $filePath = $mustacheSpecDir . $file;
    $suiteName = ltrim(substr(basename($filePath), 0, strpos(basename($filePath), '.')), "~");

    $tests = json_decode(file_get_contents($filePath), true);
    $i = 0;

    foreach( $tests['tests'] as $test ) {
        ++$i;
        $test['suiteType'] = 'mustache';
        $test['suiteName'] = $suiteName;
        $test['i'] = $i;
        hbs_generate_mustache_spec_test($test);
    }
}

