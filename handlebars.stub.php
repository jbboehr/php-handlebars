<?php

// for syntax testing, uncomment this and execute this file:
// namespace Psr\Log;
// interface LoggerInterface {}
// interface LoggerAwareInterface {}

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
class InvalidBinaryStringException extends \Exception implements Exception {}

class_alias(CompileException::class, "Handlebars\ParseException");

class Tokenizer
{
    /**
     * Tokenize a template and return an array of tokens
     *
     * @param string $tmpl
     * @return array
     */
    public static function lex(string $tmpl): array {}

    /**
     * Tokenize a template and return a readable string representation of the tokens
     *
     * @param string $tmpl
     * @return string
     */
    public static function lexPrint(string $tmpl): string {}
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
    public static function parse(string $tmpl): array {}

    /**
     * Parse a template and return a readable string representation of the AST
     *
     * @param string $tmpl
     * @return string
     * @throws \Handlebars\ParseException
     */
    public static function parsePrint(string $tmpl): string {}
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
     * @param array $options
     * @return array
     * @throws \Handlebars\CompileException
     */
    public static function compile(string $tmpl, array $options = null): array {}

    /**
     * Compile a template and return a readable string representation of the opcodes
     *
     * @param string $tmpl
     * @param array $options
     * @return string
     * @throws \Handlebars\CompileException
     */
    public static function compilePrint(string $tmpl, array $options = null): string {}
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
    public static function appendContextPath($contextPath, string $id): string {}

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
     * @return string
     * @throws \Handlebars\RuntimeException
     */
    public static function expression($value): string {}

    /**
     * Escape an expression for the output buffer. Does not handle certain
     * javascript behaviours.
     *
     * @param mixed $value
     * @return string
     * @throws \Handlebars\RuntimeException
     */
    public static function escapeExpression($value): string {}

    /**
     * Escape an expression for the output buffer. Handles certain
     * javascript behaviours.
     *
     * @param mixed $value
     * @retrun string
     * @throws \Handlebars\RuntimeException
     */
    public static function escapeExpressionCompat($value): string {}

    /**
     * Indent a multi-line string
     *
     * @param string $str
     * @param string $indent
     * @return string
     */
    static public function indent(string $str, string $indent): string {}

    /**
     * Similar to is_callable(), but only allows closures and objects
     * with an __invoke method, and removes the second and third
     * arguments.
     *
     * @param array $array
     * @return boolean
     */
    public static function isCallable($arr): bool {}

    /**
     * Is the array a numeric array?
     *
     * @param array $array
     * @return boolean
     */
    public static function isIntArray(array $arr): bool {}

    /**
     * Looks up a field in an object or array without
     * causing a notice
     *
     * @param mixed $objOrArray
     * @param string $field
     * @return mixed
     */
    public static function nameLookup($objOrArray, string $field) {}
}

class SafeString
{
    /**
     * @var string
     * This uses a typed property on >= PHP 7.4
     */
    protected /*string*/ $value;

    /**
     * Constructor
     *
     * @param string $value
     */
    public function __construct(string $value) {
        $this->value = $value;
    }

    /**
     * Magic toString method
     *
     * @return string
     */
    public function __toString(): string {
        return (string) $this->value;
    }
}

class Opcode
{
    /**
     * @var string
     * This uses a typed property on >= PHP 7.4
     */
    public /*string*/ $opcode;

    /**
     * @var array
     * This uses a typed property on >= PHP 7.4
     */
    public /*array*/ $args;

    /**
     * Constructor
     *
     * @param string $opcode
     * @param array $args
     */
    public function __construct(string $opcode, array $args) {
        $this->opcode = $opcode;
        $this->args = $args;
    }
}

class Token
{
    /**
     * @var string
     * This uses a typed property on >= PHP 7.4
     */
    public /*string*/ $name;

    /**
     * @var string
     * This uses a typed property on >= PHP 7.4
     */
    public /*string*/ $text;

    /**
     * Constructor
     *
     * @param string $name
     * @param string $text
     */
    public function __construct(string $name, string $text) {
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

	public function __construct(array $opcodes, array $children, int $blockParams) {
		$this->opcodes = $opcodes;
		$this->children = $children;
		$this->blockParams = $blockParams;
	}
}

interface Registry extends ArrayAccess, IteratorAggregate {}
class DefaultRegistry extends ArrayObject implements Registry {}

/**
 * This interface will not extend LoggerAwareInterface if the
 * psr extension is not loaded.
 */
interface Impl extends LoggerAwareInterface
{
    public function getHelpers(): ?Registry;
    public function getPartials(): ?Registry;
    public function getDecorators(): ?Registry;
    public function getLogger(): ?LoggerInterface;
    public function setHelpers(Registry $helpers): Impl;
    public function setPartials(Registry $partials): Impl;
    public function setDecorators(Registry $decorators): Impl;
    public function setLogger(LoggerInterface $logger): Impl;
    public function render(string $tmpl, $context = null, array $options = null): string;
    public function renderFile(string $filename, $context = null, array $options = null): string;
}

abstract class BaseImpl implements Impl
{
	/**
	 * @var Registry
     * This uses a typed property on >= PHP 7.4
	 */
    protected /*Registry*/ $helpers;

	/**
	 * @var Registry
     * This uses a typed property on >= PHP 7.4
	 */
    protected /*Registry*/ $partials;

	/**
	 * @var Registry
     * This uses a typed property on >= PHP 7.4
	 */
    protected /*Registry*/ $decorators;

	/**
	 * @var LoggerInterface
     * This uses a typed property on >= PHP 7.4
     * This property will not be typed if the psr extension is not loaded
	 */
    protected /*LoggerInterface*/ $logger;

    /**
     * @return Registry
     */
    public function getDecorators(): ?Registry {
        return $this->decorators;
    }

    /**
     * @return Registry
     */
    public function getHelpers(): ?Registry {
        return $this->helpers;
    }

    /**
     * @return Registry
     */
    public function getPartials(): ?Registry {
        return $this->partials;
    }

    /**
     * @return LoggerInterface
     */
    public function getLogger(): ?LoggerInterface {
        return $this->logger;
    }

    /**
     * @param Registry $helpers
     */
    public function setHelpers(Registry $helpers): Impl {
        $this->helpers = $helpers;
        return $this;
    }

    /**
     * @param Registry $partials
     */
    public function setPartials(Registry $partials): Impl {
        $this->partials = $partials;
        return $this;
    }

    /**
     * @param Registry $decorators
     */
    public function setDecorators(Registry $decorators): Impl {
        $this->decorators = $decorators;
        return $this;
    }

    /**
     * @param LoggerInterface $logger
     */
    public function setLogger(LoggerInterface $logger): Impl {
        $this->logger = $logger;
        return $this;
    }
}

class VM extends BaseImpl
{
    public function setHelpers(Registry $helpers): Impl {}

    public function setPartials(Registry $partials): Impl {}

    public function setLogger(LoggerInterface $partials): Impl {}

    public function render(string $tmpl, $context = null, array $options = null): string {}

    public function renderFile(string $filename, $context = null, array $options = null): string {}

    public function compile(string $tmpl, array $options = null): string {}

    public function renderFromBinaryString(string $binaryString, $context = null, array $options = null): string {}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
