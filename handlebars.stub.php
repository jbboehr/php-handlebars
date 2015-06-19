<?php 

namespace Handlebars;

const COMPILER_FLAG_NONE = 0;
const COMPILER_FLAG_USE_DEPTHS = (1 << 0);
const COMPILER_FLAG_STRING_PARAMS = (1 << 1);
const COMPILER_FLAG_TRACK_IDS = (1 << 2);
const COMPILER_FLAG_KNOWN_HELPERS_ONLY = (1 << 4);
const COMPILER_FLAG_COMPAT = (1 << 0);
const COMPILER_FLAG_ALL = (1 << 4) - 1;

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

    /**
     * Same as is_callable(), but does not allow simple strings and removes
     * the second and third arguments.
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

