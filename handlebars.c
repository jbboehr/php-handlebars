
/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <talloc.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "ext/standard/php_string.h"
#include "zend_exceptions.h"
#include "zend_hash.h"
#include "zend_types.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#include "php_handlebars.h"
//#include "compat.h"

#include "handlebars.h"
#include "handlebars_ast.h"
#include "handlebars_ast_list.h"
#include "handlebars_ast_printer.h"
#include "handlebars_compiler.h"
#include "handlebars_context.h"
#include "handlebars_memory.h"
#include "handlebars_opcode_printer.h"
#include "handlebars_opcodes.h"
#include "handlebars_token.h"
#include "handlebars_token_list.h"
#include "handlebars_token_printer.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"

ZEND_DECLARE_MODULE_GLOBALS(handlebars)

static void php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node, zval * current TSRMLS_DC);
static void php_handlebars_compiler_to_zval(struct handlebars_compiler * compiler, zval * current TSRMLS_DC);
static void php_handlebars_ast_list_to_zval(struct handlebars_ast_list * list, zval * current TSRMLS_DC);

static zend_class_entry * Handlebars_ce_ptr;
static zend_class_entry * HandlebarsSafeString_ce_ptr;
static zend_class_entry * HandlebarsException_ce_ptr;
static zend_class_entry * HandlebarsLexException_ce_ptr;
static zend_class_entry * HandlebarsParseException_ce_ptr;
static zend_class_entry * HandlebarsCompileException_ce_ptr;
static zend_class_entry * HandlebarsRuntimeException_ce_ptr;

/* {{{ PHP7 Compat ---------------------------------------------------------- */

#if PHP_MAJOR_VERSION < 7
#define _add_next_index_string(...) add_next_index_string(__VA_ARGS__, 1)
#define _add_assoc_string(...) add_assoc_string(__VA_ARGS__, 1)
#define _add_assoc_string_ex(...) add_assoc_string_ex(__VA_ARGS__, 1)
#define _add_assoc_stringl_ex(...) add_assoc_stringl_ex(__VA_ARGS__, 1)
#define _RETURN_STRING(a) RETURN_STRING(a, 1)
#define _RETVAL_STRING(a) RETVAL_STRING(a, 1)
#define _DECLARE_ZVAL(name) zval * name
#define _ALLOC_INIT_ZVAL(name) ALLOC_INIT_ZVAL(name)
#define _HBS_STRS ZEND_STRS
#define _zend_register_internal_class_ex(class, parent) zend_register_internal_class_ex(class, parent, NULL TSRMLS_CC)
typedef int strsize_t;
#else
#define _add_next_index_string add_next_index_string
#define _add_assoc_string(z, k, s) add_assoc_string_ex(z, k, strlen(k)+1, s)
#define _add_assoc_string_ex add_assoc_string_ex
#define _add_assoc_stringl_ex add_assoc_stringl_ex
#define _RETURN_STRING(a) RETURN_STRING(a)
#define _RETVAL_STRING(a) RETVAL_STRING(a)
#define _DECLARE_ZVAL(name) zval name ## _v; zval * name = &name ## _v
#define _ALLOC_INIT_ZVAL(name) ZVAL_NULL(name)
#define _HBS_STRS ZEND_STRL
#define _zend_register_internal_class_ex zend_register_internal_class_ex
typedef size_t strsize_t;
#endif

#define _DECLARE_ALLOC_INIT_ZVAL(name) _DECLARE_ZVAL(name); _ALLOC_INIT_ZVAL(name)

enum php_handlebars_flags {
    PHP_HANDLEBARS_FLAG_NONE = 0,
    PHP_HANDLEBARS_FLAG_PRINT = 1,
    PHP_HANDLEBARS_FLAG_EXCEPTIONS = 2
};

/* }}} ---------------------------------------------------------------------- */
/* {{{ Utils ---------------------------------------------------------------- */

#define add_assoc_handlebars_ast_node_ex(current, str, node) \
    add_assoc_handlebars_ast_node(current, _HBS_STRS(str), node TSRMLS_CC)

#define add_assoc_handlebars_ast_list_ex(current, str, list) \
    add_assoc_handlebars_ast_list(current, _HBS_STRS(str), list TSRMLS_CC)

#define add_next_index_handlebars_ast_node_ex(current, node) \
    add_next_index_handlebars_ast_node(current, node TSRMLS_CC)

static inline void add_assoc_handlebars_ast_node(zval * current, const char * key, size_t length, 
        struct handlebars_ast_node * node TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);

    if( node ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_ast_node_to_zval(node, tmp TSRMLS_CC);
        add_assoc_zval_ex(current, key, length, tmp);
    }
}

static inline void add_assoc_handlebars_ast_list(zval * current, const char * key, size_t length, 
        struct handlebars_ast_list * list TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);
    
    if( list ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_ast_list_to_zval(list, tmp TSRMLS_CC);
        add_assoc_zval_ex(current, key, length, tmp);
    }
}

static inline void add_next_index_handlebars_ast_node(zval * current, struct handlebars_ast_node * node TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);

    if( node ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_ast_node_to_zval(node, tmp TSRMLS_CC);
        add_next_index_zval(current, tmp);
    }
}

static void php_handlebars_set_error(char * msg TSRMLS_DC)
{
    if( HANDLEBARS_G(handlebars_last_error) ) {
        efree(HANDLEBARS_G(handlebars_last_error));
        HANDLEBARS_G(handlebars_last_error) = NULL;
    }
    if( msg ) {
        HANDLEBARS_G(handlebars_last_error) = estrdup(msg);
    } else {
        HANDLEBARS_G(handlebars_last_error) = NULL;
    }
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Data Conversion (inline) --------------------------------------------- */

static inline void php_handlebars_ast_list_to_zval(struct handlebars_ast_list * list, zval * current TSRMLS_DC)
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

static inline void php_handlebars_depths_to_zval(long depths, zval * current)
{
    int depthi = 1;

    array_init(current);
    
    while( depths > 0 ) {
        if( depths & 1 ) {
            add_next_index_long(current, depthi);
        }
        depthi++;
        depths = depths >> 1;
    }
}


static inline void php_handlebars_strip_to_zval(unsigned strip, zval * current)
{
    array_init(current);
    add_assoc_bool_ex(current, _HBS_STRS("left"), 1 && (strip & handlebars_ast_strip_flag_left));
    add_assoc_bool_ex(current, _HBS_STRS("right"), 1 && (strip & handlebars_ast_strip_flag_right));
    add_assoc_bool_ex(current, _HBS_STRS("openStandalone"), 1 && (strip & handlebars_ast_strip_flag_open_standalone));
    add_assoc_bool_ex(current, _HBS_STRS("closeStandalone"), 1 && (strip & handlebars_ast_strip_flag_close_standalone));
    add_assoc_bool_ex(current, _HBS_STRS("inlineStandalone"), 1 && (strip & handlebars_ast_strip_flag_inline_standalone));
    add_assoc_bool_ex(current, _HBS_STRS("leftStripped"), 1 && (strip & handlebars_ast_strip_flag_left_stripped));
    add_assoc_bool_ex(current, _HBS_STRS("rightStriped"), 1 && (strip & handlebars_ast_strip_flag_right_stripped));
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Data Conversion ------------------------------------------------------ */

static void php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);
    array_init(current);
    
    if( node == NULL ) {
        return;
    }
    
    _add_assoc_string_ex(current, _HBS_STRS("type"), (char *) handlebars_ast_node_readable_type(node->type));
    
    if( node->strip ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_strip_to_zval(node->strip, tmp);
        add_assoc_zval_ex(current, _HBS_STRS("strip"), tmp);   
    }
    
    switch( node->type ) {
        case HANDLEBARS_AST_NODE_PROGRAM: {
            add_assoc_handlebars_ast_list_ex(current, "statements", node->node.program.statements);
            break;
        }
        case HANDLEBARS_AST_NODE_MUSTACHE: {
            add_assoc_handlebars_ast_node_ex(current, "sexpr", node->node.mustache.sexpr);
            add_assoc_long_ex(current, _HBS_STRS("unescaped"), node->node.mustache.unescaped);
            break;
        }
        case HANDLEBARS_AST_NODE_SEXPR: {
            add_assoc_handlebars_ast_node_ex(current, "hash", node->node.sexpr.hash);
            add_assoc_handlebars_ast_node_ex(current, "id", node->node.sexpr.id);
            add_assoc_handlebars_ast_list_ex(current, "params", node->node.sexpr.params);
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL:
            add_assoc_handlebars_ast_node_ex(current, "partial_name", node->node.partial.partial_name);
            add_assoc_handlebars_ast_node_ex(current, "context", node->node.partial.context);
            add_assoc_handlebars_ast_node_ex(current, "hash", node->node.partial.hash);
            break;
        case HANDLEBARS_AST_NODE_RAW_BLOCK: {
            add_assoc_handlebars_ast_node_ex(current, "mustache", node->node.raw_block.mustache);
            add_assoc_handlebars_ast_node_ex(current, "program", node->node.raw_block.program);
            if( node->node.raw_block.close ) {
                _add_assoc_string_ex(current, _HBS_STRS("close"), node->node.raw_block.close);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_BLOCK: {
            add_assoc_handlebars_ast_node_ex(current, "mustache", node->node.block.mustache);
            add_assoc_handlebars_ast_node_ex(current, "program", node->node.block.program);
            add_assoc_handlebars_ast_node_ex(current, "inverse", node->node.block.inverse);
            add_assoc_handlebars_ast_node_ex(current, "close", node->node.block.close);
            add_assoc_long_ex(current, _HBS_STRS("inverted"), node->node.block.inverted);
            break;
        }
        case HANDLEBARS_AST_NODE_CONTENT: {
            if( node->node.content.string ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("string"),
                    node->node.content.string,
                    node->node.content.length);
            }
            if( node->node.content.original ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("original"),
                    node->node.content.original,
                    strlen(node->node.content.original));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_HASH: {
            add_assoc_handlebars_ast_list_ex(current, "segments", node->node.hash.segments);
            break;
        }
        case HANDLEBARS_AST_NODE_HASH_SEGMENT: {
            if( node->node.hash_segment.key ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("key"),
                    node->node.hash_segment.key,
                    node->node.hash_segment.key_length);
            }
            add_assoc_handlebars_ast_node_ex(current, "value", node->node.hash_segment.value);
            break;
        }
        case HANDLEBARS_AST_NODE_ID: {
            add_assoc_handlebars_ast_list_ex(current, "parts", node->node.id.parts);
            add_assoc_long_ex(current, _HBS_STRS("depth"), node->node.id.depth);
            add_assoc_long_ex(current, _HBS_STRS("is_simple"), node->node.id.is_simple);
            add_assoc_long_ex(current, _HBS_STRS("is_scoped"), node->node.id.is_scoped);
            if( node->node.id.id_name ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("id_name"),
                    node->node.id.id_name,
                    node->node.id.id_name_length);
            }
            if( node->node.id.string ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("string"),
                    node->node.id.string,
                    node->node.id.string_length);
            }
            if( node->node.id.original ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("original"),
                    node->node.id.original,
                    node->node.id.original_length);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL_NAME: {
            add_assoc_handlebars_ast_node_ex(current, "name", node->node.partial_name.name);
            break;
        }
        case HANDLEBARS_AST_NODE_DATA: {
            add_assoc_handlebars_ast_node_ex(current, "id", node->node.data.id);
            break;
        }
        case HANDLEBARS_AST_NODE_STRING: {
            if( node->node.string.string ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("string"),
                    node->node.string.string,
                    node->node.string.length);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_NUMBER: {
            if( node->node.number.string ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("number"),
                    node->node.number.string,
                    node->node.number.length);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_BOOLEAN: {
            if( node->node.boolean.string ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("boolean"),
                    node->node.boolean.string,
                    node->node.boolean.length);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_COMMENT: {
            if( node->node.comment.comment ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("comment"),
                    node->node.comment.comment,
                    node->node.boolean.length);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PATH_SEGMENT: {
            if( node->node.path_segment.separator ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("separator"),
                    node->node.path_segment.separator,
                    node->node.path_segment.separator_length);
            }
            if( node->node.path_segment.part ) {
                _add_assoc_stringl_ex(current, _HBS_STRS("part"),
                    node->node.path_segment.part,
                    node->node.path_segment.part_length);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_NIL:
            break;
    }
}

static void php_handlebars_operand_append_zval(struct handlebars_operand * operand, zval * arr TSRMLS_DC)
{
    switch( operand->type ) {
        case handlebars_operand_type_null:
            add_next_index_null(arr);
            break;
        case handlebars_operand_type_boolean:
            add_next_index_bool(arr, (int) operand->data.boolval);
            break;
        case handlebars_operand_type_long:
            add_next_index_long(arr, operand->data.longval);
            break;
        case handlebars_operand_type_string:
            _add_next_index_string(arr, operand->data.stringval);
            break;
        case handlebars_operand_type_array: {
            _DECLARE_ZVAL(current);
            char ** tmp = operand->data.arrayval;
            
            _ALLOC_INIT_ZVAL(current);
            array_init(current);
            
            for( ; *tmp; ++tmp ) {
                _add_next_index_string(current, *tmp);
            }
            
            add_next_index_zval(arr, current);
            break;
        }
    }
}

static void php_handlebars_opcode_to_zval(struct handlebars_opcode * opcode, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(args);
    short num = handlebars_opcode_num_operands(opcode->type);
    
    array_init(current);

    _add_assoc_string_ex(current, _HBS_STRS("opcode"), handlebars_opcode_readable_type(opcode->type));
    
    _ALLOC_INIT_ZVAL(args);
    array_init(args);
    if( num >= 1 ) {
        php_handlebars_operand_append_zval(&opcode->op1, args TSRMLS_CC);
    }
    if( num >= 2 ) {
        php_handlebars_operand_append_zval(&opcode->op2, args TSRMLS_CC);
    }
    if( num >= 3 ) {
        php_handlebars_operand_append_zval(&opcode->op3, args TSRMLS_CC);
    }
    add_assoc_zval_ex(current, _HBS_STRS("args"), args);
}

static inline void php_handlebars_opcodes_to_zval(struct handlebars_opcode ** opcodes, size_t count, zval * current TSRMLS_DC)
{
    size_t i;
    struct handlebars_opcode ** pos = opcodes;
    short num;
    _DECLARE_ZVAL(tmp);
    
    array_init(current);
    
    for( i = 0; i < count; i++, pos++ ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_opcode_to_zval(*pos, tmp TSRMLS_CC);
        add_next_index_zval(current, tmp);
    }
}

static inline void php_handlebars_compilers_to_zval(struct handlebars_compiler ** compilers, size_t count, zval * current TSRMLS_DC)
{
    size_t i;
    struct handlebars_compiler * child;
    _DECLARE_ZVAL(tmp);

    array_init(current);

    for( i = 0; i < count; i++ ) {
        child = *(compilers + i);
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_compiler_to_zval(child, tmp TSRMLS_CC);
        add_next_index_zval(current, tmp);
    }
}

static void php_handlebars_compiler_to_zval(struct handlebars_compiler * compiler, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);
    
    array_init(current);
    
    // Opcodes
    _ALLOC_INIT_ZVAL(tmp);
    php_handlebars_opcodes_to_zval(compiler->opcodes, compiler->opcodes_length, tmp TSRMLS_CC);
    add_assoc_zval_ex(current, _HBS_STRS("opcodes"), tmp);
    
    // Children
    _ALLOC_INIT_ZVAL(tmp);
    php_handlebars_compilers_to_zval(compiler->children, compiler->children_length, tmp TSRMLS_CC);
    add_assoc_zval_ex(current, _HBS_STRS("children"), tmp);
    
    // Depths
    _ALLOC_INIT_ZVAL(tmp);
    php_handlebars_depths_to_zval(compiler->depths, tmp);
    add_assoc_zval_ex(current, _HBS_STRS("depths"), tmp);
}

static char ** php_handlebars_compiler_known_helpers_from_zval(void * ctx, zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    long count = 0;
    char ** ptr;
    const char ** ptr2;
    char ** known_helpers;
    
    if( !arr || Z_TYPE_P(arr) != IS_ARRAY ) {
        return NULL;
    }
    
    data_hash = HASH_OF(arr);
    count = zend_hash_num_elements(data_hash);
    
    if( !count ) {
        return NULL;
    }

    // Count builtins >.>
    for( ptr2 = handlebars_builtins; *ptr2; ++ptr2, ++count );
    
    // Allocate array
    ptr = known_helpers = talloc_array(ctx, char *, count + 1);
        
    // Copy in known helpers
    do {
#if PHP_MAJOR_VERSION < 7
        HashPosition data_pointer = NULL;
        zval ** data_entry = NULL;
        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
            if( Z_TYPE_PP(data_entry) == IS_STRING ) {
                *ptr++ = (char *) handlebars_talloc_strdup(ctx, Z_STRVAL_PP(data_entry));
            }
            zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
#else
        zval * data_entry = NULL;
        ZEND_HASH_FOREACH_VAL(data_hash, data_entry) {
            if( Z_TYPE_P(data_entry) == IS_STRING ) {
                *ptr++ = (char *) handlebars_talloc_strdup(ctx, Z_STRVAL_P(data_entry));
            }
        } ZEND_HASH_FOREACH_END();
#endif
    } while(0);

    // Copy in builtins
    for( ptr2 = handlebars_builtins; *ptr2; ++ptr2 ) {
        *ptr++ = (char *) handlebars_talloc_strdup(ctx, *ptr2);
    }
    
    // Null terminate
    *ptr++ = NULL;

    return known_helpers;
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Functions ------------------------------------------------------------ */

/* {{{ proto string handlebars_error(void) */

static void php_handlebars_error(INTERNAL_FUNCTION_PARAMETERS)
{
    if( HANDLEBARS_G(handlebars_last_error) ) {
        _RETURN_STRING(HANDLEBARS_G(handlebars_last_error));
    }
}

PHP_FUNCTION(handlebars_error)
{
    php_handlebars_error(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Handlebars, getLastError)
{
    php_handlebars_error(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* }}} handlebars_error */
/* {{{ proto mixed handlebars_lex(string tmpl) */

static void php_handlebars_lex(INTERNAL_FUNCTION_PARAMETERS, int flags)
{
    char * tmpl;
    strsize_t tmpl_len;
    struct handlebars_context * ctx;
    struct handlebars_token_list * list;
    struct handlebars_token_list_item * el = NULL;
    struct handlebars_token_list_item * tmp = NULL;
    struct handlebars_token * token = NULL;
    char * output;
    _DECLARE_ZVAL(child);
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        return;
    }
    
    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    list = handlebars_lex(ctx);
    
    if( flags & PHP_HANDLEBARS_FLAG_PRINT ) {
        output = handlebars_token_list_print(list, 0);
        _RETVAL_STRING(output);
    } else {
        array_init(return_value);
        
        handlebars_token_list_foreach(list, el, tmp) {
            token = el->data;
            
            _ALLOC_INIT_ZVAL(child);
            array_init(child);
            _add_assoc_string_ex(child, _HBS_STRS("name"), (char *) handlebars_token_readable_type(token->token));
            if( token->text ) {
                _add_assoc_string_ex(child, _HBS_STRS("text"), token->text);
            }
            add_next_index_zval(return_value, child);
        }
    }

    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_lex)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_NONE);
}

PHP_METHOD(Handlebars, lex)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_EXCEPTIONS);
}

PHP_FUNCTION(handlebars_lex_print)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_PRINT);
}

PHP_METHOD(Handlebars, lexPrint)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_PRINT | PHP_HANDLEBARS_FLAG_EXCEPTIONS);
}

/* }}} handlebars_lex */
/* {{{ proto mixed handlebars_parse(string tmpl) */

static void php_handlebars_parse(INTERNAL_FUNCTION_PARAMETERS, int flags)
{
    char * tmpl;
    strsize_t tmpl_len;
    struct handlebars_context * ctx;
    int retval;
    char * output;
    char * errmsg;

    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        return;
    }

    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    retval = handlebars_yy_parse(ctx);

    if( ctx->error != NULL ) {
        // errmsg will be freed by the destruction of ctx
        errmsg = handlebars_context_get_errmsg(ctx);
        php_handlebars_set_error(errmsg TSRMLS_CC);
        if( flags & PHP_HANDLEBARS_FLAG_EXCEPTIONS ) {
            zend_throw_exception(HandlebarsParseException_ce_ptr, errmsg, 0 TSRMLS_CC);
        } else {
            RETVAL_FALSE;
        }
        goto done;
    } 
    
    if( flags & PHP_HANDLEBARS_FLAG_PRINT ) {
        output = handlebars_ast_print(ctx->program, 0);
        _RETVAL_STRING(output);
    } else {
        php_handlebars_ast_node_to_zval(ctx->program, return_value TSRMLS_CC);
    }
    
done:
    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_parse)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_NONE);
}

PHP_METHOD(Handlebars, parse)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_EXCEPTIONS);
}

PHP_FUNCTION(handlebars_parse_print)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_PRINT);
}

PHP_METHOD(Handlebars, parsePrint)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_PRINT | PHP_HANDLEBARS_FLAG_EXCEPTIONS);
}

/* }}} handlebars_parse */
/* {{{ proto mixed handlebars_compile(string tmpl[, long flags[, array knownHelpers]]) */

static void php_handlebars_compile(INTERNAL_FUNCTION_PARAMETERS, int flags)
{
    char * tmpl;
    strsize_t tmpl_len;
    long compile_flags = 0;
    zval * known_helpers = NULL;
    struct handlebars_context * ctx;
    struct handlebars_compiler * compiler;
    struct handlebars_opcode_printer * printer;
    int retval;
    char * errmsg;
    char ** known_helpers_arr;
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz", &tmpl, &tmpl_len, &compile_flags, &known_helpers) == FAILURE ) {
        return;
    }
    
#if PHP_MAJOR_VERSION >= 7
    // Dereference zval
    if (Z_TYPE_P(known_helpers) == IS_REFERENCE) {
        ZVAL_DEREF(known_helpers);
    }
#endif
    
    // Initialize
    ctx = handlebars_context_ctor();
    compiler = handlebars_compiler_ctor(ctx);
    printer = handlebars_opcode_printer_ctor(ctx);
    handlebars_compiler_set_flags(compiler, compile_flags);
    
    // Get known helpers
    known_helpers_arr = php_handlebars_compiler_known_helpers_from_zval(compiler, known_helpers TSRMLS_CC);
    if( known_helpers_arr ) {
        compiler->known_helpers = (const char **) known_helpers_arr;
    }
    
    // Parse
    ctx->tmpl = tmpl;
    retval = handlebars_yy_parse(ctx);
    
    if( ctx->error != NULL ) {
        // errmsg will be freed by the destruction of ctx
        errmsg = handlebars_context_get_errmsg(ctx);
        php_handlebars_set_error(errmsg TSRMLS_CC);
        if( flags & PHP_HANDLEBARS_FLAG_EXCEPTIONS ) {
            zend_throw_exception(HandlebarsParseException_ce_ptr, errmsg, 0 TSRMLS_CC);
        } else {
            RETVAL_FALSE;
        }
        goto done;
    }
    
    // Compile
    handlebars_compiler_compile(compiler, ctx->program);
    if( compiler->errnum ) {
        php_handlebars_set_error("An error occurred during compilation" TSRMLS_CC);
        if( flags & PHP_HANDLEBARS_FLAG_EXCEPTIONS ) {
            zend_throw_exception(HandlebarsCompileException_ce_ptr, "An error occurred during compilation", 0 TSRMLS_CC);
        } else {
            RETVAL_FALSE;
        }
        goto done;
    } else if( flags & PHP_HANDLEBARS_FLAG_PRINT ) {
        handlebars_opcode_printer_print(printer, compiler);
        _RETVAL_STRING(printer->output);
    } else {
        php_handlebars_compiler_to_zval(compiler, return_value TSRMLS_CC);
    }
    
done:
    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_compile)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_NONE);
}

PHP_METHOD(Handlebars, compile)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_EXCEPTIONS);
}

PHP_FUNCTION(handlebars_compile_print)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_PRINT);
}

PHP_METHOD(Handlebars, compilePrint)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_HANDLEBARS_FLAG_PRINT | PHP_HANDLEBARS_FLAG_EXCEPTIONS);
}

/* }}} handlebars_compile */
/* {{{ proto mixed handlebars_version(void) */

PHP_FUNCTION(handlebars_version)
{
    _RETURN_STRING(handlebars_version_string());
}

PHP_METHOD(Handlebars, version)
{
    _RETURN_STRING(handlebars_version_string());
}

/* }}} handlebars_version */
/* {{{ proto mixed Handlebars::nameLookup(mixed objOrArray, string field) */

static void php_handlebars_name_lookup(INTERNAL_FUNCTION_PARAMETERS)
{
    zval * obj_or_array;
    zval * zfield;
    char * field;
    long index = 0;
    strsize_t field_len;
    HashTable * data_hash;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &obj_or_array, &zfield) == FAILURE ) {
        return;
    }
    
    if( Z_TYPE_P(zfield) == IS_LONG ) {
         index = Z_LVAL_P(zfield);
    }
    convert_to_string(zfield);
    field = Z_STRVAL_P(zfield);
    field_len = Z_STRLEN_P(zfield);

#if PHP_MAJOR_VERSION < 7
    zval ** entry = NULL;
    zval * fname;
    zval * fparams[1];
    switch( Z_TYPE_P(obj_or_array) ) {
        case IS_ARRAY:
            if( index ) {
                if (zend_hash_index_find(Z_ARRVAL_P(obj_or_array), index, (void**)&entry) != FAILURE) {
                    *return_value = **entry;
                    zval_copy_ctor(return_value);
                    return;
                }
            }
            if (zend_hash_find(Z_ARRVAL_P(obj_or_array), field, field_len + 1, (void**)&entry) != FAILURE) {
                *return_value = **entry;
                zval_copy_ctor(return_value);
            }
            break;
        case IS_OBJECT:
            if( zend_hash_exists(&Z_OBJCE_P(obj_or_array)->function_table, "offsetget", sizeof("offsetget")) ) {
                // @todo can this be done without an alloc
                ALLOC_INIT_ZVAL(fname);
                ALLOC_INIT_ZVAL(fparams[0]);
                ZVAL_STRINGL(fname, "offsetget", sizeof("offsetget")-1, 0);
                ZVAL_STRINGL(fparams[0], field, field_len, 0);
                call_user_function(&Z_OBJCE_P(obj_or_array)->function_table, &obj_or_array, fname, return_value, 1, fparams TSRMLS_CC);
                efree(fname);
                efree(fparams[0]);
            } else if( Z_OBJ_HT_P(obj_or_array)->read_property != NULL ) {
                ALLOC_INIT_ZVAL(fparams[0]);
                ZVAL_STRINGL(fparams[0], field, field_len, 0);
                *return_value = *Z_OBJ_HT_P(obj_or_array)->read_property(obj_or_array, fparams[0], 0, NULL);
                zval_copy_ctor(return_value);
            } else if( Z_OBJ_HT_P(obj_or_array)->get_properties != NULL ) {
                data_hash = Z_OBJ_HT_P(obj_or_array)->get_properties(obj_or_array TSRMLS_CC);
                if (zend_hash_find(data_hash, field, field_len + 1, (void**)&entry) != FAILURE) {
                    *return_value = **entry;
                    zval_copy_ctor(return_value);
                }
            }
            break;
    }
#else
    zval * entry;
    zval fname;
    zval retval;
    zval fparams[1];
    switch( Z_TYPE_P(obj_or_array) ) {
        case IS_ARRAY:
            entry = zend_hash_str_find(Z_ARRVAL_P(obj_or_array), field, field_len);
            if (Z_TYPE_P(entry) == IS_INDIRECT) {
                entry = Z_INDIRECT_P(entry);
            }
            RETURN_ZVAL_FAST(entry);
            break;
        case IS_OBJECT:
            ZVAL_STRINGL(&fname, "offsetget", sizeof("offsetget")-1);
            if( zend_hash_str_exists(&Z_OBJCE_P(obj_or_array)->function_table, Z_STRVAL(fname), Z_STRLEN(fname)) ) {
                ZVAL_STRINGL(&fparams[0], field, field_len);
                call_user_function(&Z_OBJCE_P(obj_or_array)->function_table, obj_or_array, &fname, return_value, 1, fparams);
            } else {
                if( Z_OBJ_HT_P(obj_or_array)->get_properties != NULL ) {
                    data_hash = Z_OBJ_HT_P(obj_or_array)->get_properties(obj_or_array TSRMLS_CC);
                    entry = zend_hash_str_find(data_hash, field, field_len);
                    if (Z_TYPE_P(entry) == IS_INDIRECT) {
                        entry = Z_INDIRECT_P(entry);
                    }
                    RETURN_ZVAL_FAST(entry);
                }
            }
            break;
    }
#endif
}

PHP_FUNCTION(handlebars_name_lookup)
{
    php_handlebars_name_lookup(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Handlebars, nameLookup)
{
    php_handlebars_name_lookup(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* }}} Handlebars::nameLookup */
/* {{{ proto mixed Handlebars::isIntArray(mixed value) */

static zend_bool php_handlebars_is_int_array(zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    long count = 0;

    if( Z_TYPE_P(arr) != IS_ARRAY ) {
        return 0;
    }
    
    data_hash = HASH_OF(arr);
    count = zend_hash_num_elements(data_hash);
    
    // An empty array is an int array
    if( !count ) {
        return 1;
    }

    do {
#if PHP_MAJOR_VERSION < 7
        HashPosition data_pointer = NULL;
        zval ** data_entry = NULL;
        char * key;
        int key_len;
        long index;
        long offset = 0;
        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
            if (zend_hash_get_current_key_ex(data_hash, &key, &key_len, &index, 0, &data_pointer) == HASH_KEY_IS_STRING) {
                return 0;
            }
            // Make sure the keys are sequential
            if( index != offset++ ) {
                return 0;
            }
            zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
#else
//        zval * data_entry = NULL;
//        ZEND_HASH_FOREACH_VAL(data_hash, data_entry) {
//            if( Z_TYPE_P(data_entry) == IS_STRING ) {
//                *ptr++ = (char *) handlebars_talloc_strdup(ctx, Z_STRVAL_P(data_entry));
//            }
//        } ZEND_HASH_FOREACH_END();
#endif
    } while(0);
    
    return 1;
}

PHP_METHOD(Handlebars, isIntArray)
{
    zval * arr;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arr) == FAILURE ) {
        return;
    }
    
    if( php_handlebars_is_int_array(arr TSRMLS_CC) ) {
        RETURN_TRUE;
    } else  {
        RETURN_FALSE;
    }
}

/* }}} Handlebars::isIntArray */
/* {{{ proto mixed Handlebars::expression(mixed value) */

static zend_bool php_handlebars_expression(zval * val, zval * return_value TSRMLS_DC)
{
    zval * delim;
    switch( Z_TYPE_P(val) ) {
        case IS_BOOL:
            RETVAL_STRING(Z_BVAL_P(val) ? "true" : "false", 1);
            break;
        case IS_ARRAY:
            if( php_handlebars_is_int_array(val TSRMLS_CC) ) {
                ALLOC_INIT_ZVAL(delim);
                ZVAL_STRING(delim, ",", 0);
                php_implode(delim, val, return_value);
                efree(delim);
            } else {
                zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Trying to stringify assoc array", 0 TSRMLS_CC);
                return 0;
            }
            break;
        default:
            convert_to_string(val);
            RETVAL_ZVAL(val, 1, 0);
            break;
    }

    return 1;
}

PHP_METHOD(Handlebars, expression)
{
    zval * val;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }
    
    php_handlebars_expression(val, return_value TSRMLS_CC);
}

/* }}} Handlebars::expression */
/* {{{ proto mixed Handlebars::escapeExpression(mixed value) */

PHP_METHOD(Handlebars, escapeExpression)
{
    zval * val;
    size_t new_len;
    char * replaced;
    zval tmp;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }
    
    // @todo this should probably support inheritance
    if( Z_TYPE_P(val) == IS_OBJECT && zend_get_class_entry(val) == HandlebarsSafeString_ce_ptr ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1 TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }
    
    convert_to_string(val);
    replaced = php_escape_html_entities_ex(Z_STRVAL_P(val), Z_STRLEN_P(val), &new_len, 0, (int) ENT_QUOTES, "UTF-8", 1 TSRMLS_CC);
    RETVAL_STRING(replaced, 0);
}

/* }}} Handlebars::escapeExpression */
/* {{{ proto mixed Handlebars::escapeExpressionCompat(mixed value) */

static inline char * php_handlebars_escape_expression_replace_helper(char * input TSRMLS_DC)
{
    char * output;
    char * source;
    char * target;
    char c;
    long occurrences = 0;
    long newlen;
    long oldlen = strlen(input);
    
    if( !oldlen ) {
        return input;
    }
    
    // Count the ocurrences of ` and '
    for( source = input; c = *source; source++ ) {
        switch( c ) {
            case '`':
            case '\'':
                occurrences++;
                break;
        }
    }
    
    if( !occurrences ) {
        return input;
    }
    
    // Each occurence increases length by 5
    newlen = oldlen + (occurrences * 5) + 1;
    
    // Allocate new string
    output = target = (char *) emalloc(sizeof(char) * newlen);
    
    // Replace copy
    for( source = input; c = *source; source++ ) {
        switch( c ) {
            case '`':
                *target++ = '&';
                *target++ = '#';
                *target++ = 'x';
                *target++ = '6';
                *target++ = '0';
                *target++ = ';';
                break;
            case '\'':
                *target++ = '&';
                *target++ = '#';
                *target++ = 'x';
                *target++ = '2';
                *target++ = '7';
                *target++ = ';';
                break;
            default:
                *target++ = c;
                break;
        }
    }
    *target++ = '\0';
    
    efree(input);
    return output;
}

PHP_METHOD(Handlebars, escapeExpressionCompat)
{
    zval * val;
    size_t new_len;
    char * replaced;
    zval tmp;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }
    
    // @todo this should probably support inheritance
    if( Z_TYPE_P(val) == IS_OBJECT && zend_get_class_entry(val) == HandlebarsSafeString_ce_ptr ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1 TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }
    
    INIT_ZVAL(tmp);	
    php_handlebars_expression(val, &tmp TSRMLS_CC);

    replaced = php_escape_html_entities_ex(Z_STRVAL(tmp), Z_STRLEN(tmp), &new_len, 0, ENT_COMPAT, "UTF-8", 1 TSRMLS_CC);
    replaced = php_handlebars_escape_expression_replace_helper(replaced TSRMLS_CC);
    RETVAL_STRING(replaced, 0);
}

/* }}} Handlebars::escapeExpressionCompat */
/* {{{ proto HandlebarsSafeString::__construct(string value) */

PHP_METHOD(HandlebarsSafeString, __construct)
{
    zval * _this_zval;
    char * value;
    strsize_t value_len;

    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", 
            &_this_zval, HandlebarsSafeString_ce_ptr, &value, &value_len) == FAILURE) {
        return;
    }

	zend_update_property_stringl(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, value, value_len TSRMLS_CC);
}

/* }}} HandlebarsSafeString::__construct */
/* {{{ proto string HandlebarsSafeString::__toString() */

static inline void php_handlebars_safestring_get_value(INTERNAL_FUNCTION_PARAMETERS) {
    zval * _this_zval;
    zval * value;

    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, HandlebarsSafeString_ce_ptr) == FAILURE) {
        return;
    }

    value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, 1 TSRMLS_CC);
    RETURN_ZVAL(value, 1, 0);
}

PHP_METHOD(HandlebarsSafeString, getValue)
{
    php_handlebars_safestring_get_value(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(HandlebarsSafeString, __toString)
{
    php_handlebars_safestring_get_value(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* }}} HandlebarsSafeString::__toString */
/* {{{ Argument Info -------------------------------------------------------- */

ZEND_BEGIN_ARG_INFO_EX(handlebars_error_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_lex_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_lex_print_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_parse_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_parse_print_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_compile_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
    ZEND_ARG_INFO(0, flags)
    ZEND_ARG_ARRAY_INFO(0, knownHelpers, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_compile_print_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
    ZEND_ARG_INFO(0, flags)
    ZEND_ARG_ARRAY_INFO(0, knownHelpers, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_version_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_name_lookup_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, objOrArray)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_safestring_construct_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_safestring_get_value_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_safestring_to_string_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_is_int_array_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, arr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(handlebars_expression_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* }}} ---------------------------------------------------------------------- */
/* {{{ Function Entry ------------------------------------------------------- */

static const zend_function_entry handlebars_functions[] = {
    PHP_FE(handlebars_error, handlebars_error_args)
    PHP_FE(handlebars_lex, handlebars_lex_args)
    PHP_FE(handlebars_lex_print, handlebars_lex_print_args)
    PHP_FE(handlebars_parse, handlebars_parse_args)
    PHP_FE(handlebars_parse_print, handlebars_parse_print_args)
    PHP_FE(handlebars_compile, handlebars_compile_args)
    PHP_FE(handlebars_compile_print, handlebars_compile_print_args)
    PHP_FE(handlebars_version, handlebars_version_args)
    PHP_FE(handlebars_name_lookup, handlebars_name_lookup_args)
    PHP_FE_END
};

/* }}} ---------------------------------------------------------------------- */
/* {{{ Method Entry --------------------------------------------------------- */

static zend_function_entry Handlebars_methods[] = {
    PHP_ME(Handlebars, getLastError, handlebars_error_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, lex, handlebars_lex_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, lexPrint, handlebars_lex_print_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, parse, handlebars_lex_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, parsePrint, handlebars_lex_print_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, compile, handlebars_compile_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, compilePrint, handlebars_compile_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, version, handlebars_version_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, nameLookup, handlebars_name_lookup_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, isIntArray, handlebars_is_int_array_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, expression, handlebars_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, escapeExpression, handlebars_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Handlebars, escapeExpressionCompat, handlebars_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  { NULL, NULL, NULL }
};

static zend_function_entry HandlebarsSafeString_methods[] = {
    PHP_ME(HandlebarsSafeString, __construct, handlebars_safestring_construct_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsSafeString, getValue, handlebars_safestring_get_value_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsSafeString, __toString, handlebars_safestring_to_string_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Hooks --------------------------------------------------------- */

static PHP_GINIT_FUNCTION(handlebars)
{
    handlebars_globals->handlebars_last_error = NULL;
}

static PHP_MINIT_FUNCTION(handlebars)
{
    zend_class_entry ce;
    zend_class_entry * exception_ce = zend_exception_get_default(TSRMLS_C);
    int flags = CONST_CS | CONST_PERSISTENT;
    
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_NONE", handlebars_compiler_flag_none, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_USE_DEPTHS", handlebars_compiler_flag_use_depths, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_STRING_PARAMS", handlebars_compiler_flag_string_params, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_TRACK_IDS", handlebars_compiler_flag_track_ids, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_KNOWN_HELPERS_ONLY", handlebars_compiler_flag_known_helpers_only, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_COMPAT", handlebars_compiler_flag_compat, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_ALL", handlebars_compiler_flag_all, flags);

    // Handlebars\Native
    INIT_CLASS_ENTRY(ce, "Handlebars\\Native", Handlebars_methods);
    Handlebars_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    
    // Handlebars\Exception
    INIT_CLASS_ENTRY(ce, "Handlebars\\Exception", NULL);
    HandlebarsException_ce_ptr = _zend_register_internal_class_ex(&ce, exception_ce);

    // Handlebars\LexException
    INIT_CLASS_ENTRY(ce, "Handlebars\\LexException", NULL);
    HandlebarsLexException_ce_ptr = _zend_register_internal_class_ex(&ce, HandlebarsException_ce_ptr);

    // Handlebars\ParseException
    INIT_CLASS_ENTRY(ce, "Handlebars\\ParseException", NULL);
    HandlebarsParseException_ce_ptr = _zend_register_internal_class_ex(&ce, HandlebarsException_ce_ptr);

    // Handlebars\CompileException
    INIT_CLASS_ENTRY(ce, "Handlebars\\CompileException", NULL);
    HandlebarsCompileException_ce_ptr = _zend_register_internal_class_ex(&ce, HandlebarsException_ce_ptr);

    // Handlebars\RuntimeException
    INIT_CLASS_ENTRY(ce, "Handlebars\\RuntimeException", NULL);
    HandlebarsRuntimeException_ce_ptr = _zend_register_internal_class_ex(&ce, HandlebarsException_ce_ptr);

    // Handlebars\SafeString
    INIT_CLASS_ENTRY(ce, "Handlebars\\SafeString", HandlebarsSafeString_methods);
    HandlebarsSafeString_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(HandlebarsSafeString_ce_ptr, "value", sizeof("value")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(handlebars)
{
    php_handlebars_set_error(NULL TSRMLS_CC);
    return SUCCESS;
}

static PHP_MINFO_FUNCTION(handlebars)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Version", PHP_HANDLEBARS_VERSION);
    php_info_print_table_row(2, "Released", PHP_HANDLEBARS_RELEASE);
    php_info_print_table_row(2, "Authors", PHP_HANDLEBARS_AUTHORS);
    // @todo make spec version from libhandlebars function
    php_info_print_table_row(2, "Spec Version", PHP_HANDLEBARS_SPEC);
    php_info_print_table_row(2, "libhandlebars Version", handlebars_version_string());
    php_info_print_table_end();
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Entry --------------------------------------------------------- */

zend_module_entry handlebars_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_HANDLEBARS_NAME,                /* Name */
    handlebars_functions,               /* Functions */
    PHP_MINIT(handlebars),              /* MINIT */
    NULL,                               /* MSHUTDOWN */
    NULL,                               /* RINIT */
    PHP_RSHUTDOWN(handlebars),          /* RSHUTDOWN */
    PHP_MINFO(handlebars),              /* MINFO */
    PHP_HANDLEBARS_VERSION,             /* Version */
    PHP_MODULE_GLOBALS(handlebars),
    PHP_GINIT(handlebars),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_HANDLEBARS 
    ZEND_GET_MODULE(handlebars)      // Common for all PHP extensions which are build as shared modules  
#endif

/* }}} ---------------------------------------------------------------------- */
