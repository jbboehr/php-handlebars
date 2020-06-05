<?php

namespace Handlebars;

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
	const MUSTACHE_STYLE_LAMBDAS = 4096;
	const ALL = 8191;

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
