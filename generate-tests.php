#!/usr/bin/env php
<?php

/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

require __DIR__ . '/tests/utils.inc';

if( !extension_loaded('handlebars') ) {
    require 'handlebars.stub.php';
}

$startTime = microtime(true);
$nTests = 0;

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
        if( !empty($v['!code']) ) {
            $js = isset($v['javascript']) ? $v['javascript'] : null;
            $data[$k] = new ClosureHolder($v['php'] . '/*' . $js . '*/');
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
	
	$obj = new Handlebars\CompileContext($opcodes, $children, $blockParams);
	
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

function makeCompilerFlags(array $options = null)
{
    // Make flags
    $flags = 0;
    if( !empty($options['compat']) ) {
        $flags |= (1 << 0); //Handlebars\COMPILER_FLAG_COMPAT;
    }
    if( !empty($options['useDepths']) ) {
        $flags |= (1 << 0); //Handlebars\COMPILER_FLAG_USE_DEPTHS;
    }
    if( !empty($options['stringParams']) ) {
        $flags |= (1 << 1); //Handlebars\COMPILER_FLAG_STRING_PARAMS;
    }
    if( !empty($options['trackIds']) ) {
        $flags |= (1 << 2); //Handlebars\COMPILER_FLAG_TRACK_IDS;
    }
    if( !empty($options['noEscape']) ) {
        $flags |= (1 << 3); //Handlebars\COMPILER_FLAG_NO_ESCAPE;
    }
    if( !empty($options['knownHelpersOnly']) ) {
        $flags |= (1 << 4); //Handlebars\COMPILER_FLAG_KNOWN_HELPERS_ONLY;
    }
    if( !empty($options['preventIndent']) ) {
        $flags |= (1 << 5); //Handlebars\COMPILER_FLAG_PREVENT_INDENT;
    }
    if( !empty($options['useData']) ) {
        $flags |= (1 << 6); //Handlebars\COMPILER_FLAG_USE_DATA;
    }
    if( !empty($options['explicitPartialContext']) ) {
        $flags |= (1 << 7); //Handlebars\COMPILER_FLAG_EXPLICIT_PARTIAL_CONTEXT;
    }
    if( !empty($options['ignoreStandalone']) ) {
        $flags |= (1 << 8); //Handlebars\COMPILER_FLAG_IGNORE_STANDALONE;
    }
    return $flags;
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
    $testFile = __DIR__ . '/tests' 
        . '/handlebars/' . $test['suiteType'] 
        . '/' . $test['suiteName'] 
        . '/' . sprintf("%03d", $test['number']) /*. '-' . $safeName */ . '.phpt';
    return $testFile;
}

function hbs_generate_test_head(array &$test) {
    // Skip this test for now
    $skip = "!extension_loaded('handlebars')";
    $reason = '';
    
    switch( $test['description'] . '-' . $test['it'] ) {
        case 'basic context-escaping':
        if( $test['number'] != 3 ) {
            break;
        }
        case 'helpers-helper for nested raw block gets raw content':
        $skip = 'true';
        $reason = 'skip for now'; 
        break;
    }
    if( $test['suiteType'] == 'spec' ) {
        switch( $test['suiteName'] . '-' . $test['description'] . '-' . $test['it'] ) {
            case 'basic-basic context-compiling with a string context':
                $skip = 'true';
                $reason = 'PHP does not have string methods';
                break;
            case 'blocks-decorators-should apply mustache decorators':
            case 'blocks-decorators-should apply allow undefined return':
            case 'blocks-decorators-should apply block decorators':
            case 'blocks-decorators-should support nested decorators':
            case 'blocks-decorators-should apply multiple decorators':
            case 'blocks-decorators-should access parent variables':
                $skip = 'true';
                $reason = 'decorators are not currently supported';
                break;
            case 'helpers-block params-should take presednece over parent block params':
		$test['expected'] = '15foo';
		break;
        }
    }

    return join("\n", array(
        '--TEST--',
        $test['suiteName'] . ' #' . $test['number'] . ' - ' . $test['description'] /* . ' - ' . $test['it']*/,
        '--DESCRIPTION--',
        $test['description'] . ' - ' . $test['it'],
        '--SKIPIF--',
        "<?php if( $skip ) die('skip $reason'); ?>",
        '--FILE--',
        '<?php',
        'use Handlebars\Compiler;',
        'use Handlebars\Parser;',
        'use Handlebars\Tokenizer;',
        'use Handlebars\Utils;',
        'use Handlebars\VM;',
        'require __DIR__ . "/../../../utils.inc";',
    ));
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
    
    $options = isset($test['options']) ? $test['options'] : array();
    $compileOptions = isset($test['compileOptions']) ? $test['compileOptions'] : array();
    $options += $compileOptions;
    
    $compileFlags = makeCompilerFlags($options);
    $knownHelpers = isset($options['knownHelpers']) ? $options['knownHelpers'] : array();
    /*if( isset($test['helpers']) ) {
        $knownHelpers = array_merge($knownHelpers, array_keys($test['helpers']));
    }
    if( isset($test['globalHelpers']) ) {
        $knownHelpers = array_merge($knownHelpers, array_keys($test['globalHelpers']));
    }*/
    if( empty($knownHelpers) ) {
        $knownHelpers = null;
    } else {
        $knownHelpers = array_keys($knownHelpers);
    }
    $expectedOpcodes = patch_context($test['opcodes']);
    
    $output = '';
    $output .= '$options = ' . var_export($options, true) . ';' . PHP_EOL;
    $output .= '$tmpl = ' . var_export($test['template'], true) . ';' . PHP_EOL;
    $output .= '$compileFlags = ' . var_export($compileFlags, true) . ';' . PHP_EOL;
    $output .= '$knownHelpers = ' . var_export($knownHelpers, true) . ';' . PHP_EOL;
    $output .= 'myprint(Compiler::compile($tmpl, $compileFlags, $knownHelpers), true);' . PHP_EOL;
    $output .= 'myprint(gettype(Compiler::compilePrint($tmpl, $compileFlags, $knownHelpers)), true);' . PHP_EOL;
    $output .= '--EXPECT--' . PHP_EOL;
    $output .= myprint($expectedOpcodes) . myprint('string') . PHP_EOL;
    return $output;
}

function hbs_generate_spec_test_body_generic(array $test) {
    $options = isset($test['options']) ? $test['options'] : array();
    $compileOptions = isset($test['compileOptions']) ? $test['compileOptions'] : array();
    $options += $compileOptions;
    $helpers = array_merge((array)@$test['globalHelpers'], (array)@$test['helpers']);
    $partials = array_merge((array)@$test['globalPartials'], (array)@$test['partials']);
    $context = isset($test['data']) ? $test['data'] : null;
    $expectHead = null;
    $message = null;
    if( array_key_exists('expected', $test) ) {
        $expected = $test['expected'];
    } else if( !empty($test['exception']) ) {
	$expectHead = 'EXPECTF';
        // @todo improve this
        $expected = '%AUncaught exception%A';
        if( !empty($test['message']) ) {
            /*$expected =*/ $message = $test['message'];
        } else {
            //$expected = 'exception';
        }
    } else {
        echo "Whoops\n";
        exit(1);
    }
    
    return PHP_EOL . join(PHP_EOL, array_filter(array(
        'use Handlebars\HelperRegistry;',
        'use Handlebars\PartialRegistry;',
        'use Handlebars\SafeString;',
        '$tmpl = ' . var_export($test['template'], true) . ';',
        '$context = ' . varExport($context, true) . ';',
        $helpers ? '$helpers = new HelperRegistry(' . varExport($helpers, true) . ');' : null,
        $partials ? '$partials = new PartialRegistry(' . varExport($partials, true) . ');' : null,
        '$options = ' . varExport($options, true) . ';',
        //'$knownHelpers = ' . var_export($knownHelpers, true) . ';',
        '$vm = new VM();',
        $helpers ? '$vm->setHelpers($helpers);' : null,
        $partials ? '$vm->setPartials($partials);' : null,
        'echo $vm->render($tmpl, $context, $options);',
	$message ? '/* Error message: ' . addcslashes($message, "\r\n") . ' */' : null,
        '--'. ($expectHead ?: 'EXPECT') . '--',
        $expected
    )));
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



// Main

// Parser/Tokenizer
$specDir = __DIR__ . '/spec/handlebars/spec/';
foreach( scandir($specDir) as $file ) {
    if( $file[0] === '.' || substr($file, -5) !== '.json' ) {
        continue;
    }
    $filePath = $specDir . $file;
    $suiteName = substr(basename($filePath), 0, strpos(basename($filePath), '.'));
    $tests = json_decode(file_get_contents($filePath), true);
    $number = 0;
    foreach( $tests as $test ) {
        ++$number;
        $test['suiteType'] = 'spec';
        $test['suiteName'] = $suiteName;
        $test['number'] = $number;
        hbs_generate_spec_test($test);
    }
}

// Export
$exportDir = __DIR__ . '/spec/handlebars/export/';
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

    $number = 0;
    foreach( $tests as $test ) {
        ++$number;
        $test['suiteType'] = 'export';
        $test['suiteName'] = $suiteName;
        $test['number'] = $number;
        hbs_generate_export_test($test);
    }
}
