
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "handlebars.h"
#include "handlebars_ast.h"
#include "handlebars_ast_list.h"
#include "handlebars_ast_printer.h"
#include "handlebars_compiler.h"
#include "handlebars_context.h"
#include "handlebars_memory.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsParser_ce_ptr;

static void php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node, zval * current TSRMLS_DC);
static void php_handlebars_ast_list_to_zval(struct handlebars_ast_list * list, zval * current TSRMLS_DC);
/* }}} Variables & Prototypes */

/* {{{ Array Utils */
#define add_assoc_handlebars_ast_node_ex(current, str, node) \
    add_assoc_handlebars_ast_node(current, PHP5TO7_STRL(str), node TSRMLS_CC)

#define add_assoc_handlebars_ast_list_ex(current, str, list) \
    add_assoc_handlebars_ast_list(current, PHP5TO7_STRL(str), list TSRMLS_CC)

#define add_next_index_handlebars_ast_node_ex(current, node) \
    add_next_index_handlebars_ast_node(current, node TSRMLS_CC)

static zend_always_inline void add_assoc_handlebars_ast_node(zval * current, const char * key, size_t length,
        struct handlebars_ast_node * node TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);

    if( node ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_ast_node_to_zval(node, tmp TSRMLS_CC);
        add_assoc_zval_ex(current, key, length, tmp);
    }
}

static zend_always_inline void add_assoc_handlebars_ast_list(zval * current, const char * key, size_t length,
        struct handlebars_ast_list * list TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);

    if( list ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_ast_list_to_zval(list, tmp TSRMLS_CC);
        add_assoc_zval_ex(current, key, length, tmp);
    }
}

static zend_always_inline void add_next_index_handlebars_ast_node(zval * current, struct handlebars_ast_node * node TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);

    if( node ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_ast_node_to_zval(node, tmp TSRMLS_CC);
        add_next_index_zval(current, tmp);
    }
}
/* }}} Array Utils */

/* {{{ Conversion Utils (inline) */
static zend_always_inline void php_handlebars_ast_list_to_zval(struct handlebars_ast_list * list, zval * current TSRMLS_DC)
{
    struct handlebars_ast_list_item * item;
    struct handlebars_ast_list_item * ltmp;
    _DECLARE_ZVAL(tmp);

    if( list != NULL ) {
        array_init(current);

        handlebars_ast_list_foreach(list, item, ltmp) {
            add_next_index_handlebars_ast_node_ex(current, item->data);
        }
    }
}

static zend_always_inline void php_handlebars_strip_to_zval(unsigned strip, zval * current)
{
    array_init(current);
    add_assoc_bool_ex(current, PHP5TO7_STRL("left"), 1 && (strip & handlebars_ast_strip_flag_left));
    add_assoc_bool_ex(current, PHP5TO7_STRL("right"), 1 && (strip & handlebars_ast_strip_flag_right));
    add_assoc_bool_ex(current, PHP5TO7_STRL("openStandalone"), 1 && (strip & handlebars_ast_strip_flag_open_standalone));
    add_assoc_bool_ex(current, PHP5TO7_STRL("closeStandalone"), 1 && (strip & handlebars_ast_strip_flag_close_standalone));
    add_assoc_bool_ex(current, PHP5TO7_STRL("inlineStandalone"), 1 && (strip & handlebars_ast_strip_flag_inline_standalone));
    add_assoc_bool_ex(current, PHP5TO7_STRL("leftStripped"), 1 && (strip & handlebars_ast_strip_flag_left_stripped));
    add_assoc_bool_ex(current, PHP5TO7_STRL("rightStriped"), 1 && (strip & handlebars_ast_strip_flag_right_stripped));
}

static zend_always_inline void php_handlebars_ast_node_add_path_params_hash(struct handlebars_ast_node * node, zval * current TSRMLS_DC)
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

static zend_always_inline void php_handlebars_ast_node_add_literal(struct handlebars_ast_node_literal * literal, zval * current TSRMLS_DC)
{
    if( literal->value ) {
        php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("value"), literal->value);
    }
    if( literal->original ) {
    	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("original"), literal->original);
    }
}

static zend_always_inline void php_handlebars_loc_to_zval(struct handlebars_locinfo * locinfo, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(start);
    _DECLARE_ZVAL(end);

    _ALLOC_INIT_ZVAL(start);
    array_init(start);
    add_assoc_long_ex(start, PHP5TO7_STRL("line"), locinfo->first_line);
    add_assoc_long_ex(start, PHP5TO7_STRL("column"), locinfo->first_column);

    _ALLOC_INIT_ZVAL(end);
    array_init(end);
    add_assoc_long_ex(end, PHP5TO7_STRL("line"), locinfo->last_line);
    add_assoc_long_ex(end, PHP5TO7_STRL("column"), locinfo->last_column);

    array_init(current);
    add_assoc_zval_ex(current, PHP5TO7_STRL("start"), start);
    add_assoc_zval_ex(current, PHP5TO7_STRL("end"), end);
}
/* }}} Conversion Utils (inline) */

/* {{{ Conversion Utils */
static void php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);
    _DECLARE_ZVAL(tmp2);
    array_init(current);

    if( node == NULL ) {
        return;
    }

    php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("type"), (char *) handlebars_ast_node_readable_type(node->type));

    // Strip
    if( node->strip ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_strip_to_zval(node->strip, tmp);
        add_assoc_zval_ex(current, PHP5TO7_STRL("strip"), tmp);
    }

    // Locinfo
    _ALLOC_INIT_ZVAL(tmp);
    php_handlebars_loc_to_zval(&node->loc, tmp TSRMLS_CC);
    add_assoc_zval_ex(current, PHP5TO7_STRL("loc"), tmp);

    // Main
    switch( node->type ) {
        case HANDLEBARS_AST_NODE_PROGRAM: {
            add_assoc_handlebars_ast_list_ex(current, "statements", node->node.program.statements);
            add_assoc_long_ex(current, PHP5TO7_STRL("chained"), node->node.program.chained);
            if( node->node.program.block_param1 ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("block_param1"), node->node.program.block_param1);
            }
            if( node->node.program.block_param2 ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("block_param2"), node->node.program.block_param2);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_MUSTACHE: {
            php_handlebars_ast_node_add_path_params_hash(node, current TSRMLS_CC);
            add_assoc_long_ex(current, PHP5TO7_STRL("unescaped"), node->node.mustache.unescaped);
            break;
        }
        case HANDLEBARS_AST_NODE_SEXPR: {
            php_handlebars_ast_node_add_path_params_hash(node, current TSRMLS_CC);
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL:
            php_handlebars_ast_node_add_path_params_hash(node, current TSRMLS_CC);
            break;
        case HANDLEBARS_AST_NODE_RAW_BLOCK: {
            php_handlebars_ast_node_add_path_params_hash(node, current TSRMLS_CC);
            if( node->node.raw_block.program ) {
                add_assoc_handlebars_ast_node_ex(current, "program", node->node.raw_block.program);
            }
            if( node->node.raw_block.inverse ) {
                add_assoc_handlebars_ast_node_ex(current, "inverse", node->node.raw_block.inverse);
            }
            add_assoc_long_ex(current, PHP5TO7_STRL("open_strip"), node->node.raw_block.open_strip);
            add_assoc_long_ex(current, PHP5TO7_STRL("inverse_strip"), node->node.raw_block.inverse_strip);
            add_assoc_long_ex(current, PHP5TO7_STRL("close_strip"), node->node.raw_block.close_strip);
            break;
        }
        case HANDLEBARS_AST_NODE_BLOCK: {
            php_handlebars_ast_node_add_path_params_hash(node, current TSRMLS_CC);
            if( node->node.raw_block.program ) {
                add_assoc_handlebars_ast_node_ex(current, "program", node->node.block.program);
            }
            if( node->node.raw_block.inverse ) {
                add_assoc_handlebars_ast_node_ex(current, "inverse", node->node.block.inverse);
            }
            add_assoc_long_ex(current, PHP5TO7_STRL("open_strip"), node->node.block.open_strip);
            add_assoc_long_ex(current, PHP5TO7_STRL("inverse_strip"), node->node.block.inverse_strip);
            add_assoc_long_ex(current, PHP5TO7_STRL("close_strip"), node->node.block.close_strip);
            break;
        }
        case HANDLEBARS_AST_NODE_CONTENT: {
            php_handlebars_ast_node_add_literal(&node->node.content, current TSRMLS_CC);
            break;
        }
        case HANDLEBARS_AST_NODE_HASH: {
            add_assoc_handlebars_ast_list_ex(current, "pairs", node->node.hash.pairs);
            break;
        }
        case HANDLEBARS_AST_NODE_HASH_PAIR: {
            if( node->node.hash_pair.key ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("key"),
                    node->node.hash_pair.key);
            }
            add_assoc_handlebars_ast_node_ex(current, "value", node->node.hash_pair.value);
            break;
        }
        case HANDLEBARS_AST_NODE_PATH: {
            add_assoc_handlebars_ast_list_ex(current, "parts", node->node.path.parts);
            if( node->node.path.original ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("original"),
                    node->node.path.original);
            }
            add_assoc_long_ex(current, PHP5TO7_STRL("depth"), node->node.path.depth);
            add_assoc_long_ex(current, PHP5TO7_STRL("data"), node->node.path.data);
            add_assoc_long_ex(current, PHP5TO7_STRL("falsy"), node->node.path.falsy);
            break;
        }
        case HANDLEBARS_AST_NODE_STRING: {
            php_handlebars_ast_node_add_literal(&node->node.string, current TSRMLS_CC);
            break;
        }
        case HANDLEBARS_AST_NODE_NUMBER: {
            php_handlebars_ast_node_add_literal(&node->node.number, current TSRMLS_CC);
            break;
        }
        case HANDLEBARS_AST_NODE_BOOLEAN: {
            php_handlebars_ast_node_add_literal(&node->node.boolean, current TSRMLS_CC);
            break;
        }
        case HANDLEBARS_AST_NODE_NUL: {
            php_handlebars_ast_node_add_literal(&node->node.nul, current TSRMLS_CC);
            break;
        }
        case HANDLEBARS_AST_NODE_UNDEFINED: {
            php_handlebars_ast_node_add_literal(&node->node.undefined, current TSRMLS_CC);
            break;
        }
        case HANDLEBARS_AST_NODE_COMMENT: {
            if( node->node.comment.value ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("value"), node->node.comment.value);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PATH_SEGMENT: {
            if( node->node.path_segment.separator ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("separator"),
                    node->node.path_segment.separator);
            }
            if( node->node.path_segment.part ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("part"),
                    node->node.path_segment.part);
            }
            if( node->node.path_segment.original ) {
            	php5to7_add_assoc_string_ex(current, PHP5TO7_STRL("original"),
                    node->node.path_segment.original);
            }
            break;
        }
        // These should never happen
        case HANDLEBARS_AST_NODE_INTERMEDIATE:
        case HANDLEBARS_AST_NODE_INVERSE:
        case HANDLEBARS_AST_NODE_NIL:
            break;
    }
}
/* }}} Conversion Utils */

/* {{{ proto mixed Handlebars\Parser::parse(string tmpl) */
static zend_always_inline void php_handlebars_parse(INTERNAL_FUNCTION_PARAMETERS, short print)
{
    char * tmpl;
    strsize_t tmpl_len;
    struct handlebars_context * ctx;
    int retval;
    char * output;
    char * errmsg;

#ifndef FAST_ZPP
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        return;
    }
#else
    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STRING(tmpl, tmpl_len)
    ZEND_PARSE_PARAMETERS_END();
#endif

    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    retval = handlebars_yy_parse(ctx);

    if( ctx->error ) {
        // errmsg will be freed by the destruction of ctx
        errmsg = handlebars_context_get_errmsg(ctx);
        zend_throw_exception(HandlebarsParseException_ce_ptr, errmsg, ctx->errnum TSRMLS_CC);
        goto done;
    } else if( ctx->errnum ) {
        zend_throw_exception(HandlebarsCompileException_ce_ptr, "An error occurred during parsing", ctx->errnum TSRMLS_CC);
        goto done;
    }

    if( print ) {
        output = handlebars_ast_print(ctx->program, 0);
        PHP5TO7_RETVAL_STRING(output);
    } else {
        php_handlebars_ast_node_to_zval(ctx->program, return_value TSRMLS_CC);
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
ZEND_BEGIN_ARG_INFO_EX(HandlebarsParser_parse_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsParser methods */
static zend_function_entry HandlebarsParser_methods[] = {
    PHP_ME(HandlebarsParser, parse, HandlebarsParser_parse_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsParser, parsePrint, HandlebarsParser_parse_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
{ NULL, NULL, NULL }
};
/* }}} HandlebarsParser methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_parser)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Parser", HandlebarsParser_methods);
    HandlebarsParser_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

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
