<?php

namespace Handlebars;

use ArrayAccess;
use IteratorAggregate;
use ArrayObject;

/**
 * extension version
 */
const VERSION = "0.9.1";

/**
 * libhandlebars version
 */
const LIBVERSION = "0.7.2";

/**
 * libhandlebars version (at compile time)
 */
const LIBVERSION2 = "0.7.2";

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

interface Registry extends ArrayAccess, IteratorAggregate {}
class DefaultRegistry extends ArrayObject implements Registry {}

// ! These are not actually exposed by the extension, just a hack
if (class_exists('Psr\\Log\\LoggerAwareInterface')) {
    interface LoggerAwareInterface extends Psr\Log\LoggerAwareInterface {}
} else {
    interface LoggerAwareInterface {}
}
if (class_exists('Psr\\Log\\LoggerInterface')) {
    interface LoggerInterface extends Psr\Log\LoggerInterface {}
} else {
    interface LoggerInterface {}
}

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
