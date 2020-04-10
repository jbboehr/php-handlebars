<?php

namespace Handlebars;

use ArrayAccess;
use IteratorAggregate;
use ArrayObject;
use Psr\Log\LoggerInterface;
use Psr\Log\LoggerAwareInterface;

/**
 * extension version
 */
const VERSION = "x.y.z";

/**
 * libhandlebars version
 */
const LIBVERSION = "x.y.z";

/**
 * If PSR extension is available
 */
const PSR = true;

/**
 * The cache backend name
 */
const CACHE_BACKEND = 'mmap';

interface Exception {}
class CompileException extends \Exception implements Exception {}
class InvalidArgumentException extends \InvalidArgumentException implements Exception {}
class RuntimeException extends \RuntimeException implements Exception {}

class_alias(CompileException::class, "Handlebars\ParseException");

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
	const STRICT = 1024;
	const ASSUME_OBJECTS = 2048;
	const COMPAT = 1;
	const ALL = 4095;

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
     * Indent a multi-line string
     *
     * @param string $str
     * @param string $indent
     * @return string
     */
    static public function indent($str, $indent) {}

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

class Program
{
	/**
	 * @var Opcode[]
	 */
	public $opcodes;

	/**
	 * @var Program[]
	 */
	public $children;

	/**
	 * @var Program[]
	 */
	public $decorators;

	/**
	 * @var boolean
	 */
	public $isSimple;

	/**
	 * @var boolean
	 */
	public $useDepths;

	/**
	 * @var boolean
	 */
	public $usePartial;

	/**
	 * @var boolean
	 */
	public $useDecorators;

	/**
	 * @var integer
	 */
	public $blockParams;

	/**
	 * @var boolean
	 */
	public $stringParams;

	/**
	 * @var boolean
	 */
	public $trackIds;

	public function __construct(array $opcodes, array $children, $blockParams) {
		$this->opcodes = $opcodes;
		$this->children = $children;
		$this->blockParams = $blockParams;
	}
}

interface Registry extends ArrayAccess, IteratorAggregate {}
class DefaultRegistry extends ArrayObject implements Registry {}

interface Impl extends LoggerAwareInterface
{
    public function getHelpers();
    public function getPartials();
    public function getDecorators();
    public function getLogger();
    public function setHelpers(Registry $helpers);
    public function setPartials(Registry $partials);
    public function setDecorators(Registry $decorators);
    public function setLogger(LoggerInterface $logger);
    public function render($tmpl, $context = null, array $options = null);
    public function renderFile($filename, $context = null, array $options = null);
}

abstract class BaseImpl implements Impl
{
	/**
	 * @var Registry
	 */
    protected $helpers;

	/**
	 * @var Registry
	 */
    protected $partials;

	/**
	 * @var Registry
	 */
    protected $decorators;

	/**
	 * @var LoggerInterface
	 */
    protected $logger;

    /**
     * @return Registry
     */
    public function getDecorators() {
        return $this->decorators;
    }

    /**
     * @return Registry
     */
    public function getHelpers() {
        return $this->helpers;
    }

    /**
     * @return Registry
     */
    public function getPartials() {
        return $this->partials;
    }

    /**
     * @return LoggerInterface
     */
    public function getLogger() {
        return $this->logger;
    }

    /**
     * @param Registry $helpers
     */
    public function setHelpers(Registry $helpers) {
        $this->helpers = $helpers;
    }

    /**
     * @param Registry $partials
     */
    public function setPartials(Registry $partials) {
        $this->partials = $partials;
    }

    /**
     * @param Registry $decorators
     */
    public function setDecorators(Registry $decorators) {
        $this->decorators = $decorators;
    }

    /**
     * @param LoggerInterface $logger
     */
    public function setLogger(LoggerInterface $logger) {
        $this->logger = $logger;
    }
}

class VM extends BaseImpl
{
    public function setHelpers(Registry $helpers) {}

    public function setPartials(Registry $partials) {}

    public function setLogger(LoggerInterface $partials) {}

    public function render($tmpl, $context = null, array $options = null) {}

    public function renderFile($filename, $context = null, array $options = null) {}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
