<?php 

namespace Handlebars;

const COMPILER_FLAG_NONE = 0;
const COMPILER_FLAG_USE_DEPTHS = (1 << 0);
const COMPILER_FLAG_STRING_PARAMS = (1 << 1);
const COMPILER_FLAG_TRACK_IDS = (1 << 2);
const COMPILER_FLAG_KNOWN_HELPERS_ONLY = (1 << 4);
const COMPILER_FLAG_COMPAT = (1 << 0);
const COMPILER_FLAG_ALL = (1 << 4) - 1;

class Native
{
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
     */
    public static function parse($tmpl) {}

    /**
     * Parse a template and return a readable string representation of the AST
     * 
     * @param string $tmpl
     * @return string
     */
    public static function parsePrint($tmpl) {}

    /**
     * Compile a template and return the opcodes 
     * 
     * @param string $tmpl
     * @param integer $flags
     * @param array $knownHelpers
     * @return array
     */
    public static function compile($tmpl, $flags = 0, array $knownHelpers = null) {}

    /**
     * Compile a template and return a readable string representation of the opcodes 
     * 
     * @param string $tmpl
     * @param integer $flags
     * @param array $knownHelpers
     * @return string
     */
    public static function compilePrint($tmpl, $flags = 0, array $knownHelpers = null) {}
}

