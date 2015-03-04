<?php 

const HANDLEBARS_COMPILER_FLAG_NONE = 0;
const HANDLEBARS_COMPILER_FLAG_USE_DEPTHS = (1 << 0);
const HANDLEBARS_COMPILER_FLAG_STRING_PARAMS = (1 << 1);
const HANDLEBARS_COMPILER_FLAG_TRACK_IDS = (1 << 2);
const HANDLEBARS_COMPILER_FLAG_KNOWN_HELPERS_ONLY = (1 << 4);
const HANDLEBARS_COMPILER_FLAG_COMPAT = (1 << 0);
const HANDLEBARS_COMPILER_FLAG_ALL = (1 << 4) - 1;

/**
 * Get the last error that occurred.
 * 
 * @return string
 */
function handlebars_error() {}

/**
 * Tokenize a template and return an array of tokens
 *
 * @param string $tmpl
 * @return array
 */
function handlebars_lex($tmpl) {}

/**
 * Tokenize a template and return a readable string representation of the tokens
 *
 * @param string $tmpl
 * @return string
 */
function handlebars_lex_print($tmpl) {}

/**
 * Parse a template and return the AST
 *
 * @param string $tmpl
 * @return array
 */
function handlebars_parse($tmpl) {}

/**
 * Parse a template and return a readable string representation of the AST
 * 
 * @param string $tmpl
 * @return string
 */
function handlebars_parse($tmpl) {}

/**
 * Compile a template and return the opcodes 
 * 
 * @param string $tmpl
 * @param integer $flags
 * @param array $knownHelpers
 * @return array
 */
function handlebars_compile($tmpl, $flags = 0, array $knownHelpers = null) {}

/**
 * Compile a template and return a readable string representation of the opcodes 
 * 
 * @param string $tmpl
 * @param integer $flags
 * @param array $knownHelpers
 * @return string
 */
function handlebars_compile_print($tmpl, $flags = 0, array $knownHelpers = null) {}
