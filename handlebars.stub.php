<?php 

namespace Handlebars;

const COMPILER_FLAG_NONE = 0;
const COMPILER_FLAG_USE_DEPTHS = 1;
const COMPILER_FLAG_STRING_PARAMS = 2;
const COMPILER_FLAG_TRACK_IDS = 4;
const COMPILER_FLAG_NO_ESCAPE = 8;
const COMPILER_FLAG_KNOWN_HELPERS_ONLY = 16;
const COMPILER_FLAG_PREVENT_INDENT = 32;
const COMPILER_FLAG_USE_DATA = 64;
const COMPILER_FLAG_EXPLICIT_PARTIAL_CONTEXT = 128;
const COMPILER_FLAG_IGNORE_STANDALONE = 256;
const COMPILER_FLAG_ALTERNATE_DECORATORS = 512;
const COMPILER_FLAG_COMPAT = 1;
const COMPILER_FLAG_ALL = 1023;

const COMPILER_RESULT_FLAG_USE_DEPTHS = 1;
const COMPILER_RESULT_FLAG_USE_PARTIAL = 2;
const COMPILER_RESULT_FLAG_IS_SIMPLE = 4;
const COMPILER_RESULT_FLAG_USE_DECORATORS = 8;
const COMPILER_RESULT_FLAG_ALL = 15;

class Exception extends \Exception {}
class LexException extends Exception {}
class ParseException extends Exception {}
class CompileException extends Exception {}
class RuntimeException extends Exception {}

class Native
{
    /**
     * extension version
     */
    const VERSION = "x.y.z";

    /**
     * libhandlebars version
     */
    const LIBVERSION = "x.y.z";
    
    /**
     * Get the last error that occurred.
     * 
     * @return string
     */
    public static function getLastError() {}

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
    public function __construct($value) {}

    /**
     * Magic toString method
     *
     * @return string
     */
    public function __toString() {}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */

