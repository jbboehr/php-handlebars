<?php 

namespace Handlebars;

/**
 * extension version
 */
const VERSION = "x.y.z";

/**
 * libhandlebars version
 */
const LIBVERSION = "x.y.z";

interface Exception {}
class LexException extends \Exception implements Exception {}
class ParseException extends \Exception implements Exception {}
class CompileException extends \Exception implements Exception {}
class InvalidArgumentException extends \InvalidArgumentException implements Exception {}
class RuntimeException extends \RuntimeException implements Exception {}

class Tokenizer
{
    /**
     * Tokenize a template and return an array of tokens
     *
     * @param string $tmpl
     * @return array
     */
    public static function lex($tmpl) {}

    /**
     * Tokenize a template and return a readable string representation of the tokens
     *
     * @param string $tmpl
     * @return string
     */
    public static function lexPrint($tmpl) {}
}

class Parser
{
    /**
     * Parse a template and return the AST
     *
     * @param string $tmpl
     * @return array
     * @throws \Handlebars\ParseException
     */
    public static function parse($tmpl) {}

    /**
     * Parse a template and return a readable string representation of the AST
     * 
     * @param string $tmpl
     * @return string
     * @throws \Handlebars\ParseException
     */
    public static function parsePrint($tmpl) {}
}

class Compiler
{
	const NONE = 0;
	
	const USE_DEPTHS = 1;
	const STRING_PARAMS = 2;
	const TRACK_IDS = 4;
	const NO_ESCAPE = 8;
	const KNOWN_HELPERS_ONLY = 16;
	const PREVENT_INDENT = 32;
	const USE_DATA = 64;
	const EXPLICIT_PARTIAL_CONTEXT = 128;
	const IGNORE_STANDALONE = 256;
	const ALTERNATE_DECORATORS = 512;
	const COMPAT = 1;
	const ALL = 1023;
	
	const USE_PARTIAL = 2;
	const IS_SIMPLE = 4;
	const USE_DECORATORS = 8;
	const RESULT_ALL = 15;
	
    /**
     * Compile a template and return the opcodes 
     * 
     * @param string $tmpl
     * @param integer $flags
     * @param array $knownHelpers
     * @return array
     * @throws \Handlebars\CompileException
     */
    public static function compile($tmpl, $flags = 0, array $knownHelpers = null) {}

    /**
     * Compile a template and return a readable string representation of the opcodes 
     * 
     * @param string $tmpl
     * @param integer $flags
     * @param array $knownHelpers
     * @return string
     * @throws \Handlebars\CompileException
     */
    public static function compilePrint($tmpl, $flags = 0, array $knownHelpers = null) {}
}

class Utils
{
    /**
     * Append context path for trackIds
     *
     * @param mixed $contextPath
     * @param string $id
     * @return string
     */
    public static function appendContextPath($contextPath, $id) {}

    /**
     * @param mixed $value
     * @return array
     */
    public static function createFrame($value) {}

    /**
     * Prepare an expression for the output buffer. Handles certain
     * javascript behaviours.
     *
     * @param mixed $value
     * @retrun string
     * @throws \Handlebars\RuntimeException
     */
    public static function expression($value) {}

    /**
     * Escape an expression for the output buffer. Does not handle certain
     * javascript behaviours.
     *
     * @param mixed $value
     * @retrun string
     * @throws \Handlebars\RuntimeException
     */
    public static function escapeExpression($value) {}

    /**
     * Escape an expression for the output buffer. Handles certain
     * javascript behaviours.
     *
     * @param mixed $value
     * @retrun string
     * @throws \Handlebars\RuntimeException
     */
    public static function escapeExpressionCompat($value) {}

    /**
     * Similar to is_callable(), but only allows closures and objects
     * with an __invoke method, and removes the second and third 
     * arguments.
     *
     * @param array $array
     * @return boolean
     */
    public static function isCallable($arr) {}

    /**
     * Is the array a numeric array?
     *
     * @param array $array
     * @return boolean
     */
    public static function isIntArray($arr) {}

    /**
     * Looks up a field in an object or array without
     * causing a notice
     * 
     * @param mixed $objOrArray
     * @param string $field
     * @return mixed
     */
    public static function nameLookup($objOrArray, $field) {}
}

class SafeString
{
    /**
     * @var string
     */
    protected $value;

    /**
     * Constructor
     *
     * @param string $value
     */
    public function __construct($value) {
        $this->value = $value;
    }

    /**
     * Magic toString method
     *
     * @return string
     */
    public function __toString() {
        return (string) $this->value;
    }
}

class Opcode
{
    /**
     * @var string
     */
    public $opcode;

    /**
     * @var array
     */
    public $args;

    /**
     * Constructor
     *
     * @param string $opcode
     * @param array $args
     */
    public function __construct($opcode, array $args) {
        $this->opcode = $opcode;
        $this->args = $args;
    }
}

class Token
{
    /**
     * @var string
     */
    public $name;

    /**
     * @var string
     */
    public $text;

    /**
     * Constructor
     *
     * @param string $name
     * @param string $text
     */
    public function __construct($name, $text) {
        $this->name = $name;
        $this->text = $text;
    }
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
