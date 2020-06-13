
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>

#include "Zend/zend_API.h"
#include "Zend/zend_exceptions.h"
#include "main/php.h"

#define HANDLEBARS_AST_PRIVATE
#define HANDLEBARS_AST_LIST_PRIVATE

#include "handlebars.h"
#include "handlebars_memory.h"
#include "handlebars_ast.h"
#include "handlebars_ast_list.h"
#include "handlebars_ast_printer.h"
#include "handlebars_compiler.h"
#include "handlebars_parser.h"
#include "handlebars_string.h"

#define BOOLEAN HBS_BOOLEAN
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsParser_ce_ptr;

static void php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node, zval * current);
static void php_handlebars_ast_list_to_zval(struct handlebars_ast_list * list, zval * current);
/* }}} Variables & Prototypes */

/* {{{ Array Utils */
#define add_assoc_handlebars_ast_node_ex(current, str, node) \
    add_assoc_handlebars_ast_node(current, ZEND_STRL(str), node)

#define add_assoc_handlebars_ast_list_ex(current, str, list) \
    add_assoc_handlebars_ast_list(current, ZEND_STRL(str), list)

#define add_next_index_handlebars_ast_node_ex(current, node) \
    add_next_index_handlebars_ast_node(current, node)

static zend_always_inline void add_assoc_handlebars_ast_node(zval * current, const char * key, size_t length,
        struct handlebars_ast_node * node)
{
    if( node ) {
        zval tmp = {0};
        ZVAL_NULL(&tmp);
        php_handlebars_ast_node_to_zval(node, &tmp);
        add_assoc_zval_ex(current, key, length, &tmp);
    }
}

static zend_always_inline void add_assoc_handlebars_ast_list(zval * current, const char * key, size_t length,
        struct handlebars_ast_list * list)
{
    if( list ) {
        zval tmp = {0};
        ZVAL_NULL(&tmp);
        php_handlebars_ast_list_to_zval(list, &tmp);
        add_assoc_zval_ex(current, key, length, &tmp);
    }
}

static zend_always_inline void add_next_index_handlebars_ast_node(zval * current, struct handlebars_ast_node * node)
{
    if( node ) {
        zval tmp = {0};
        ZVAL_NULL(&tmp);
        php_handlebars_ast_node_to_zval(node, &tmp);
        add_next_index_zval(current, &tmp);
    }
}
/* }}} Array Utils */

/* {{{ Conversion Utils (inline) */
static zend_always_inline void php_handlebars_ast_list_to_zval(struct handlebars_ast_list * list, zval * current)
{
    struct handlebars_ast_list_item * item;
    struct handlebars_ast_list_item * tmp;

    if( list != NULL ) {
        array_init(current);

        handlebars_ast_list_foreach(list, item, tmp) {
            add_next_index_handlebars_ast_node_ex(current, item->data);
        }
    }
}

static zend_always_inline void php_handlebars_strip_to_zval(unsigned strip, zval * current)
{
    array_init(current);
    add_assoc_bool_ex(current, ZEND_STRL("left"), 0 != (strip & handlebars_ast_strip_flag_left));
    add_assoc_bool_ex(current, ZEND_STRL("right"), 0 != (strip & handlebars_ast_strip_flag_right));
    add_assoc_bool_ex(current, ZEND_STRL("openStandalone"), 0 != (strip & handlebars_ast_strip_flag_open_standalone));
    add_assoc_bool_ex(current, ZEND_STRL("closeStandalone"), 0 != (strip & handlebars_ast_strip_flag_close_standalone));
    add_assoc_bool_ex(current, ZEND_STRL("inlineStandalone"), 0 != (strip & handlebars_ast_strip_flag_inline_standalone));
    add_assoc_bool_ex(current, ZEND_STRL("leftStripped"), 0 != (strip & handlebars_ast_strip_flag_left_stripped));
    add_assoc_bool_ex(current, ZEND_STRL("rightStriped"), 0 != (strip & handlebars_ast_strip_flag_right_stripped));
}

static zend_always_inline void php_handlebars_ast_node_add_path_params_hash(struct handlebars_ast_node * node, zval * current)
{
    struct handlebars_ast_node * path = handlebars_ast_node_get_path(node);
    struct handlebars_ast_list * params = handlebars_ast_node_get_params(node);
    struct handlebars_ast_node * hash = handlebars_ast_node_get_hash(node);
    if( path ) {
        if( node->type == HANDLEBARS_AST_NODE_PARTIAL ) {
            add_assoc_handlebars_ast_node_ex(current, "name", path);
        } else {
            add_assoc_handlebars_ast_node_ex(current, "path", path);
        }
    }
    if( params ) {
        add_assoc_handlebars_ast_list_ex(current, "params", params);
    }
    if( hash ) {
        add_assoc_handlebars_ast_node_ex(current, "hash", hash);
    }
}

static zend_always_inline void php_handlebars_ast_node_add_literal(struct handlebars_ast_node_literal * literal, zval * current)
{
    if( literal->value ) {
        add_assoc_stringl_ex(current, ZEND_STRL("value"), hbs_str_val(literal->value), hbs_str_len(literal->value));
    }
    if( literal->original ) {
    	add_assoc_stringl_ex(current, ZEND_STRL("original"), hbs_str_val(literal->original), hbs_str_len(literal->original));
    }
}

static zend_always_inline void php_handlebars_loc_to_zval(struct handlebars_locinfo * locinfo, zval * current)
{
    zval start = {0};
    zval end = {0};

    ZVAL_NULL(&start);
    array_init(&start);
    add_assoc_long_ex(&start, ZEND_STRL("line"), locinfo->first_line);
    add_assoc_long_ex(&start, ZEND_STRL("column"), locinfo->first_column);

    ZVAL_NULL(&end);
    array_init(&end);
    add_assoc_long_ex(&end, ZEND_STRL("line"), locinfo->last_line);
    add_assoc_long_ex(&end, ZEND_STRL("column"), locinfo->last_column);

    array_init(current);
    add_assoc_zval_ex(current, ZEND_STRL("start"), &start);
    add_assoc_zval_ex(current, ZEND_STRL("end"), &end);
}
/* }}} Conversion Utils (inline) */

/* {{{ Conversion Utils */
static void php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node, zval * current)
{
    zval tmp = {0};
    array_init(current);

    if( node == NULL ) {
        return;
    }

    add_assoc_string_ex(current, ZEND_STRL("type"), (char *) handlebars_ast_node_readable_type(node->type));

    // Strip
    if( node->strip ) {
        ZVAL_NULL(&tmp);
        php_handlebars_strip_to_zval(node->strip, &tmp);
        add_assoc_zval_ex(current, ZEND_STRL("strip"), &tmp);
    }

    // Locinfo
    ZVAL_NULL(&tmp);
    php_handlebars_loc_to_zval(&node->loc, &tmp);
    add_assoc_zval_ex(current, ZEND_STRL("loc"), &tmp);

    // Main
    switch( node->type ) {
        case HANDLEBARS_AST_NODE_PROGRAM: {
            add_assoc_handlebars_ast_list_ex(current, "statements", node->node.program.statements);
            add_assoc_long_ex(current, ZEND_STRL("chained"), node->node.program.chained);
            if( node->node.program.block_param1 ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("block_param1"),
                                             HBS_STR_STRL(node->node.program.block_param1));
            }
            if( node->node.program.block_param2 ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("block_param2"),
                                             HBS_STR_STRL(node->node.program.block_param2));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_MUSTACHE: {
            php_handlebars_ast_node_add_path_params_hash(node, current);
            add_assoc_long_ex(current, ZEND_STRL("unescaped"), node->node.mustache.unescaped);
            break;
        }
        case HANDLEBARS_AST_NODE_SEXPR: {
            php_handlebars_ast_node_add_path_params_hash(node, current);
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL:
            php_handlebars_ast_node_add_path_params_hash(node, current);
            break;
        case HANDLEBARS_AST_NODE_RAW_BLOCK: {
            php_handlebars_ast_node_add_path_params_hash(node, current);
            if( node->node.raw_block.program ) {
                add_assoc_handlebars_ast_node_ex(current, "program", node->node.raw_block.program);
            }
            if( node->node.raw_block.inverse ) {
                add_assoc_handlebars_ast_node_ex(current, "inverse", node->node.raw_block.inverse);
            }
            add_assoc_long_ex(current, ZEND_STRL("open_strip"), node->node.raw_block.open_strip);
            add_assoc_long_ex(current, ZEND_STRL("inverse_strip"), node->node.raw_block.inverse_strip);
            add_assoc_long_ex(current, ZEND_STRL("close_strip"), node->node.raw_block.close_strip);
            break;
        }
        case HANDLEBARS_AST_NODE_BLOCK: {
            php_handlebars_ast_node_add_path_params_hash(node, current);
            if( node->node.raw_block.program ) {
                add_assoc_handlebars_ast_node_ex(current, "program", node->node.block.program);
            }
            if( node->node.raw_block.inverse ) {
                add_assoc_handlebars_ast_node_ex(current, "inverse", node->node.block.inverse);
            }
            add_assoc_long_ex(current, ZEND_STRL("open_strip"), node->node.block.open_strip);
            add_assoc_long_ex(current, ZEND_STRL("inverse_strip"), node->node.block.inverse_strip);
            add_assoc_long_ex(current, ZEND_STRL("close_strip"), node->node.block.close_strip);
            break;
        }
        case HANDLEBARS_AST_NODE_CONTENT: {
            php_handlebars_ast_node_add_literal(&node->node.content, current);
            break;
        }
        case HANDLEBARS_AST_NODE_HASH: {
            add_assoc_handlebars_ast_list_ex(current, "pairs", node->node.hash.pairs);
            break;
        }
        case HANDLEBARS_AST_NODE_HASH_PAIR: {
            if( node->node.hash_pair.key ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("key"), HBS_STR_STRL(node->node.hash_pair.key));
            }
            add_assoc_handlebars_ast_node_ex(current, "value", node->node.hash_pair.value);
            break;
        }
        case HANDLEBARS_AST_NODE_PATH: {
            add_assoc_handlebars_ast_list_ex(current, "parts", node->node.path.parts);
            if( node->node.path.original ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("original"), HBS_STR_STRL(node->node.path.original));
            }
            add_assoc_long_ex(current, ZEND_STRL("depth"), node->node.path.depth);
            add_assoc_long_ex(current, ZEND_STRL("data"), node->node.path.data);
            add_assoc_long_ex(current, ZEND_STRL("falsy"), node->node.path.falsy);
            break;
        }
        case HANDLEBARS_AST_NODE_STRING: {
            php_handlebars_ast_node_add_literal(&node->node.string, current);
            break;
        }
        case HANDLEBARS_AST_NODE_NUMBER: {
            php_handlebars_ast_node_add_literal(&node->node.number, current);
            break;
        }
        case HANDLEBARS_AST_NODE_BOOLEAN: {
            php_handlebars_ast_node_add_literal(&node->node.boolean, current);
            break;
        }
        case HANDLEBARS_AST_NODE_NUL: {
            php_handlebars_ast_node_add_literal(&node->node.nul, current);
            break;
        }
        case HANDLEBARS_AST_NODE_UNDEFINED: {
            php_handlebars_ast_node_add_literal(&node->node.undefined, current);
            break;
        }
        case HANDLEBARS_AST_NODE_COMMENT: {
            if( node->node.comment.value ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("value"), HBS_STR_STRL(node->node.comment.value));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PATH_SEGMENT: {
            if( node->node.path_segment.separator ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("separator"), HBS_STR_STRL(node->node.path_segment.separator));
            }
            if( node->node.path_segment.part ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("part"), HBS_STR_STRL(node->node.path_segment.part));
            }
            if( node->node.path_segment.original ) {
            	add_assoc_stringl_ex(current, ZEND_STRL("original"), HBS_STR_STRL(node->node.path_segment.original));
            }
            break;
        }

        // These should never happen
        // LCOV_EXCL_START
        case HANDLEBARS_AST_NODE_INTERMEDIATE:
        case HANDLEBARS_AST_NODE_INVERSE:
        case HANDLEBARS_AST_NODE_NIL:
        case HANDLEBARS_AST_NODE_PARTIAL_BLOCK: // ?
            break;
        default: assert(0); break;
        // LCOV_EXCL_STOP
    }
}
/* }}} Conversion Utils */

/* {{{ proto mixed Handlebars\Parser::parse(string tmpl) */
static void php_handlebars_parse(INTERNAL_FUNCTION_PARAMETERS, short print)
{
    zend_string * tmpl = NULL;
    struct handlebars_context * ctx;
    struct handlebars_parser * parser;
    struct handlebars_string * output;
    jmp_buf buf;
    struct handlebars_string * tmpl_str;
    struct handlebars_ast_node * ast;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STR(tmpl)
    ZEND_PARSE_PARAMETERS_END();

    ctx = handlebars_context_ctor();

    // Note: there is no flags argument here, so we can't preprocess the delimiters

    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
    parser = handlebars_parser_ctor(ctx);

    // Parse
    tmpl_str = handlebars_string_ctor(HBSCTX(parser), ZSTR_VAL(tmpl), ZSTR_LEN(tmpl));
    php_handlebars_try(HandlebarsCompileException_ce_ptr, parser, &buf);
    ast = handlebars_parse_ex(parser, tmpl_str, 0);

    // Print or convert to zval
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, parser, &buf);
    if( print ) {
        output = handlebars_ast_print(HBSCTX(parser), ast);
        HBS_RETVAL_STR(output);
    } else {
        php_handlebars_ast_node_to_zval(ast, return_value);
    }

done:
    handlebars_context_dtor(ctx);
}

PHP_METHOD(HandlebarsParser, parse)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_METHOD(HandlebarsParser, parsePrint)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} Handlebars\Parser::parse */

/* {{{ Argument Info */
PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsParser, parse, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsParser, parsePrint, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsParser methods */
static zend_function_entry HandlebarsParser_methods[] = {
    PHP_ME(HandlebarsParser, parse, arginfo_HandlebarsParser_parse, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsParser, parsePrint, arginfo_HandlebarsParser_parsePrint, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} HandlebarsParser methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_parser)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Parser", HandlebarsParser_methods);
    HandlebarsParser_ce_ptr = zend_register_internal_class(&ce);

    return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
