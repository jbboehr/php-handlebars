
/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
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

static zend_class_entry * HandlebarsNative_ce_ptr;
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
#define _INIT_ZVAL INIT_ZVAL
#define _ALLOC_INIT_ZVAL(name) ALLOC_INIT_ZVAL(name)
#define _HBS_STRS ZEND_STRS
#define _zend_read_property(a, b, c, d, e, f) zend_read_property(a, b, c, d, e)
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
#define _INIT_ZVAL ZVAL_NULL
#define _ALLOC_INIT_ZVAL(name) ZVAL_NULL(name)
#define _HBS_STRS ZEND_STRL
#define _zend_read_property(a, b, c, d, e, f) zend_read_property(a, b, c, d, e, f)
#define _zend_register_internal_class_ex zend_register_internal_class_ex
typedef size_t strsize_t;
#endif

#define _DECLARE_ALLOC_INIT_ZVAL(name) _DECLARE_ZVAL(name); _ALLOC_INIT_ZVAL(name)

/* }}} ---------------------------------------------------------------------- */
/* {{{ Utils ---------------------------------------------------------------- */

#define add_assoc_handlebars_ast_node_ex(current, str, node) \
    add_assoc_handlebars_ast_node(current, _HBS_STRS(str), node TSRMLS_CC)

#define add_assoc_handlebars_ast_list_ex(current, str, list) \
    add_assoc_handlebars_ast_list(current, _HBS_STRS(str), list TSRMLS_CC)

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

static zend_always_inline void php_handlebars_set_error(char * msg TSRMLS_DC)
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

/*
static zend_always_inline void php_handlebars_depths_to_zval(long depths, zval * current)
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
*/

static zend_always_inline void php_handlebars_strip_to_zval(unsigned strip, zval * current)
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
        _add_assoc_string_ex(current, _HBS_STRS("value"), literal->value);
    }
    if( literal->original ) {
        _add_assoc_string_ex(current, _HBS_STRS("original"), literal->original);
    }
}

static zend_always_inline void php_handlebars_loc_to_zval(struct handlebars_locinfo * locinfo, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(start);
    _DECLARE_ZVAL(end);

    _ALLOC_INIT_ZVAL(start);
    array_init(start);
    add_assoc_long_ex(start, _HBS_STRS("line"), locinfo->first_line);
    add_assoc_long_ex(start, _HBS_STRS("column"), locinfo->first_column);

    _ALLOC_INIT_ZVAL(end);
    array_init(end);
    add_assoc_long_ex(end, _HBS_STRS("line"), locinfo->last_line);
    add_assoc_long_ex(end, _HBS_STRS("column"), locinfo->last_column);

    array_init(current);
    add_assoc_zval_ex(current, _HBS_STRS("start"), start);
    add_assoc_zval_ex(current, _HBS_STRS("end"), end);
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Data Conversion ------------------------------------------------------ */

static void php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(tmp);
    _DECLARE_ZVAL(tmp2);
    array_init(current);
    
    if( node == NULL ) {
        return;
    }
    
    _add_assoc_string_ex(current, _HBS_STRS("type"), (char *) handlebars_ast_node_readable_type(node->type));
    
    // Strip
    if( node->strip ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_strip_to_zval(node->strip, tmp);
        add_assoc_zval_ex(current, _HBS_STRS("strip"), tmp);   
    }
    
    // Locinfo
    _ALLOC_INIT_ZVAL(tmp);
    php_handlebars_loc_to_zval(&node->loc, tmp TSRMLS_CC);
    add_assoc_zval_ex(current, _HBS_STRS("loc"), tmp);

    // Main
    switch( node->type ) {
        case HANDLEBARS_AST_NODE_PROGRAM: {
            add_assoc_handlebars_ast_list_ex(current, "statements", node->node.program.statements);
            add_assoc_long_ex(current, _HBS_STRS("chained"), node->node.program.chained);
            if( node->node.program.block_param1 ) {
            	_add_assoc_string_ex(current, _HBS_STRS("block_param1"), node->node.program.block_param1);
            }
            if( node->node.program.block_param2 ) {
            	_add_assoc_string_ex(current, _HBS_STRS("block_param2"), node->node.program.block_param2);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_MUSTACHE: {
        	php_handlebars_ast_node_add_path_params_hash(node, current TSRMLS_CC);
            add_assoc_long_ex(current, _HBS_STRS("unescaped"), node->node.mustache.unescaped);
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
            add_assoc_long_ex(current, _HBS_STRS("open_strip"), node->node.raw_block.open_strip);
            add_assoc_long_ex(current, _HBS_STRS("inverse_strip"), node->node.raw_block.inverse_strip);
            add_assoc_long_ex(current, _HBS_STRS("close_strip"), node->node.raw_block.close_strip);
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
            add_assoc_long_ex(current, _HBS_STRS("open_strip"), node->node.block.open_strip);
            add_assoc_long_ex(current, _HBS_STRS("inverse_strip"), node->node.block.inverse_strip);
            add_assoc_long_ex(current, _HBS_STRS("close_strip"), node->node.block.close_strip);
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
                _add_assoc_string_ex(current, _HBS_STRS("key"),
                    node->node.hash_pair.key);
            }
            add_assoc_handlebars_ast_node_ex(current, "value", node->node.hash_pair.value);
            break;
        }
        case HANDLEBARS_AST_NODE_PATH: {
            add_assoc_handlebars_ast_list_ex(current, "parts", node->node.path.parts);
            if( node->node.path.original ) {
                _add_assoc_string_ex(current, _HBS_STRS("original"),
                    node->node.path.original);
            }
            add_assoc_long_ex(current, _HBS_STRS("depth"), node->node.path.depth);
            add_assoc_long_ex(current, _HBS_STRS("data"), node->node.path.data);
            add_assoc_long_ex(current, _HBS_STRS("falsy"), node->node.path.falsy);
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
                _add_assoc_string_ex(current, _HBS_STRS("value"), node->node.comment.value);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PATH_SEGMENT: {
            if( node->node.path_segment.separator ) {
                _add_assoc_string_ex(current, _HBS_STRS("separator"),
                    node->node.path_segment.separator);
            }
            if( node->node.path_segment.part ) {
                _add_assoc_string_ex(current, _HBS_STRS("part"),
                    node->node.path_segment.part);
            }
            if( node->node.path_segment.original ) {
                _add_assoc_string_ex(current, _HBS_STRS("original"),
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

    _add_assoc_string_ex(current, _HBS_STRS("opcode"), (char *) handlebars_opcode_readable_type(opcode->type));
    
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

static zend_always_inline void php_handlebars_opcodes_to_zval(
    struct handlebars_opcode ** opcodes, size_t count, zval * current TSRMLS_DC)
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

static zend_always_inline void php_handlebars_compilers_to_zval(
    struct handlebars_compiler ** compilers, size_t count, zval * current TSRMLS_DC)
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

    // useDepths
    // @todo make sure this works
    if( compiler->use_depths ) {
    	add_assoc_bool_ex(current, _HBS_STRS("useDepths"), compiler->use_depths);
    }
    
    // BlockParams
    // @todo fix
    add_assoc_long_ex(current, _HBS_STRS("blockParams"), 0);
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

/* {{{ proto string Handlebars\Native::getLastError(void) */

PHP_METHOD(HandlebarsNative, getLastError)
{
    if( HANDLEBARS_G(handlebars_last_error) ) {
        _RETURN_STRING(HANDLEBARS_G(handlebars_last_error));
    }
}

/* }}} Handlebars\Native::getLastError */
/* {{{ proto mixed Handlebars\Native::lex(string tmpl) */

static zend_always_inline void php_handlebars_lex(INTERNAL_FUNCTION_PARAMETERS, short print)
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
    
    if( print ) {
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

PHP_METHOD(HandlebarsNative, lex)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_METHOD(HandlebarsNative, lexPrint)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* }}} Handlebars\Native::lex */
/* {{{ proto mixed Handlebars\Native::parse(string tmpl) */

static zend_always_inline void php_handlebars_parse(INTERNAL_FUNCTION_PARAMETERS, short print)
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
        zend_throw_exception(HandlebarsParseException_ce_ptr, errmsg, 0 TSRMLS_CC);
        goto done;
    } 
    
    if( print ) {
        output = handlebars_ast_print(ctx->program, 0);
        _RETVAL_STRING(output);
    } else {
        php_handlebars_ast_node_to_zval(ctx->program, return_value TSRMLS_CC);
    }
    
done:
    handlebars_context_dtor(ctx);
}

PHP_METHOD(HandlebarsNative, parse)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_METHOD(HandlebarsNative, parsePrint)
{
    php_handlebars_parse(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* }}} Handlebars\Native::parse */
/* {{{ proto mixed Handlebars\Native::compile(string tmpl[, long flags[, array knownHelpers]]) */

static zend_always_inline void php_handlebars_compile(INTERNAL_FUNCTION_PARAMETERS, short print)
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
        zend_throw_exception(HandlebarsParseException_ce_ptr, errmsg, 0 TSRMLS_CC);
        goto done;
    }
    
    // Compile
    handlebars_compiler_compile(compiler, ctx->program);
    if( compiler->errnum ) {
        // @todo provide a better error message
        php_handlebars_set_error("An error occurred during compilation" TSRMLS_CC);
        zend_throw_exception(HandlebarsCompileException_ce_ptr, "An error occurred during compilation", 0 TSRMLS_CC);
        goto done;
    }

    if( print ) {
        handlebars_opcode_printer_print(printer, compiler);
        _RETVAL_STRING(printer->output);
    } else {
        php_handlebars_compiler_to_zval(compiler, return_value TSRMLS_CC);
    }
    
done:
    handlebars_context_dtor(ctx);
}

PHP_METHOD(HandlebarsNative, compile)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_METHOD(HandlebarsNative, compilePrint)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* }}} Handlebars\Native::compile */
/* {{{ proto mixed Handlebars\Native::nameLookup(mixed value, string field) */

#if PHP_MAJOR_VERSION < 7
static zend_always_inline void php_handlebars_name_lookup(zval * value, zval * field, zval * return_value TSRMLS_DC)
{
    long index = -1;
    HashTable * data_hash;
    zval ** entry = NULL;
    zval * fname;
    zval * prop;
    zval * params[1];

    // Support integer keys
    if( Z_TYPE_P(field) == IS_LONG ) {
        index = Z_LVAL_P(field);
        convert_to_string(field);
    } else {
        convert_to_string(field);
        if( is_numeric_string(Z_STRVAL_P(field), Z_STRLEN_P(field), NULL, NULL, 0) ) {
            sscanf(Z_STRVAL_P(field), "%ld", &index);
        }
    }

    if( Z_TYPE_P(value) == IS_ARRAY ) {
        if( index > -1 && zend_hash_index_find(Z_ARRVAL_P(value), index, (void**)&entry) != FAILURE ) {
            *return_value = **entry;
            zval_copy_ctor(return_value);
        } else if( zend_hash_find(Z_ARRVAL_P(value), Z_STRVAL_P(field), Z_STRLEN_P(field) + 1, (void**)&entry) != FAILURE ) {
            *return_value = **entry;
            zval_copy_ctor(return_value);
        }
    } else if( Z_TYPE_P(value) == IS_OBJECT ) {
        ALLOC_INIT_ZVAL(prop);
        ZVAL_STRINGL(prop, Z_STRVAL_P(field), Z_STRLEN_P(field), 0);
        if( zend_hash_exists(&Z_OBJCE_P(value)->function_table, "offsetget", sizeof("offsetget")) ) {
            ALLOC_INIT_ZVAL(fname);
            ZVAL_STRINGL(fname, "offsetget", sizeof("offsetget")-1, 0);
            params[0] = prop;
            call_user_function(&Z_OBJCE_P(value)->function_table, &value, fname, return_value, 1, params TSRMLS_CC);
            efree(fname);
        } else if( Z_OBJ_HT_P(value)->read_property != NULL ) {
#if PHP_API_VERSION > 20090626
            *return_value = *Z_OBJ_HT_P(value)->read_property(value, prop, 0, NULL TSRMLS_CC);
#else
            *return_value = *Z_OBJ_HT_P(value)->read_property(value, prop, 0 TSRMLS_CC);
#endif
            zval_copy_ctor(return_value);
        }
        efree(prop);
    }
}
#else
static zend_always_inline void php_handlebars_name_lookup(zval * value, zval * field, zval * return_value TSRMLS_DC)
{
    long long_index;
    zend_long index = -1;
    zval * entry = NULL;
    zval retval;
    zval fname;
    zval prop;
    HashTable * data_hash;

    // Support integer keys
    if( Z_TYPE_P(field) == IS_LONG ) {
        index = Z_LVAL_P(field);
        convert_to_string(field);
    } else {
        convert_to_string(field);
        if( is_numeric_string(Z_STRVAL_P(field), Z_STRLEN_P(field), NULL, NULL, 0) ) {
            sscanf(Z_STRVAL_P(field), "%ld", &long_index);
            index = (zend_long) long_index;
        }
    }

    if( Z_TYPE_P(value) == IS_ARRAY ) {
        if( index > -1 && (entry = zend_hash_index_find(Z_ARRVAL_P(value), index)) ) {
            // nothing
        } else {
            entry = zend_hash_str_find(Z_ARRVAL_P(value), Z_STRVAL_P(field), Z_STRLEN_P(field));
        }
    } else if( Z_TYPE_P(value) == IS_OBJECT ) {
        ZVAL_STRINGL(&fname, "offsetget", sizeof("offsetget")-1);
        ZVAL_STRINGL(&prop, Z_STRVAL_P(field), Z_STRLEN_P(field));
        if( zend_hash_str_exists(&Z_OBJCE_P(value)->function_table, Z_STRVAL(fname), Z_STRLEN(fname)) ) {
            call_user_function(&Z_OBJCE_P(value)->function_table, value, &fname, return_value, 1, &prop);
        } else if( Z_OBJ_HT_P(value)->read_property != NULL ) {
            entry = Z_OBJ_HT_P(value)->read_property(value, &prop, 0, NULL, NULL TSRMLS_CC);
        }
        zval_dtor(&fname);
        zval_dtor(&prop);
    }

    if( entry ) {
        if (Z_TYPE_P(entry) == IS_INDIRECT) {
            entry = Z_INDIRECT_P(entry);
        }
        RETURN_ZVAL_FAST(entry);
    }
}
#endif

PHP_METHOD(HandlebarsNative, nameLookup)
{
    zval * value;
    zval * field;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &value, &field) == FAILURE ) {
        return;
    }
    
    php_handlebars_name_lookup(value, field, return_value TSRMLS_CC);
}

/* }}} Handlebars\Native::nameLookup */
/* {{{ proto boolean Handlebars\Native::isIntArray(mixed value) */

#if PHP_MAJOR_VERSION < 7
static zend_always_inline zend_bool php_handlebars_is_int_array(zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    HashPosition data_pointer = NULL;
    zval ** data_entry = NULL;
    char * key;
    int key_len;
    long index;
    long idx = 0;

    if( Z_TYPE_P(arr) != IS_ARRAY ) {
        return 0;
    }
    
    data_hash = Z_ARRVAL_P(arr);

    // An empty array is an int array
    if( !zend_hash_num_elements(data_hash) ) {
        return 1;
    }

    zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
    while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
        if (zend_hash_get_current_key_ex(data_hash, &key, &key_len, &index, 0, &data_pointer) == HASH_KEY_IS_STRING) {
            return 0;
        }
        // Make sure the keys are sequential
        if( index != idx++ ) {
            return 0;
        }
        zend_hash_move_forward_ex(data_hash, &data_pointer);
    }

    return 1;
}
#else
static zend_always_inline zend_bool php_handlebars_is_int_array(zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
	zend_string * key;
	zend_ulong index;
	zend_ulong idx = 0;

    if( Z_TYPE_P(arr) != IS_ARRAY ) {
        return 0;
    }

    data_hash = Z_ARRVAL_P(arr);
    
    // An empty array is an int array
    if( !zend_hash_num_elements(data_hash) ) {
        return 1;
    }

	ZEND_HASH_FOREACH_KEY(data_hash, index, key) {
        if( key ) {
            return 0;
        } else {
            if( index != idx ) {
                return 0;
            }
        }
        idx++;
    } ZEND_HASH_FOREACH_END();
    
    return 1;
}
#endif

PHP_METHOD(HandlebarsNative, isIntArray)
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

/* }}} Handlebars\Native::isIntArray */
/* {{{ proto string Handlebars\Native::expression(mixed value) */

#if PHP_MAJOR_VERSION < 7
static zend_always_inline zend_bool php_handlebars_expression(zval * val, zval * return_value TSRMLS_DC)
{
    zval delim;

    switch( Z_TYPE_P(val) ) {
        case IS_BOOL:
            _RETVAL_STRING(Z_BVAL_P(val) ? "true" : "false");
            break;
        case IS_ARRAY:
            if( php_handlebars_is_int_array(val TSRMLS_CC) ) {
                ZVAL_STRING(&delim, ",", 0);
                php_implode(&delim, val, return_value TSRMLS_CC);
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
#else
static zend_always_inline zend_bool php_handlebars_expression(zval * val, zval * return_value TSRMLS_DC)
{
    zend_string * delim;
    
    switch( Z_TYPE_P(val) ) {
        case IS_TRUE:
            _RETVAL_STRING("true");
            break;
        case IS_FALSE:
            _RETVAL_STRING("false");
            break;
        case IS_ARRAY:
            if( php_handlebars_is_int_array(val TSRMLS_CC) ) {
                delim = zend_string_init(",", 1, 0);
                php_implode(delim, val, return_value TSRMLS_CC);
                zend_string_free(delim);
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
#endif

PHP_METHOD(HandlebarsNative, expression)
{
    zval * val;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }
    
    php_handlebars_expression(val, return_value TSRMLS_CC);
}

/* }}} Handlebars\Native::expression */
/* {{{ proto string Handlebars\Native::escapeExpression(mixed value) */

#if PHP_MAJOR_VERSION < 7
static zend_always_inline void php_handlebars_escape_expression(zval * val, zval * return_value TSRMLS_DC)
{
    size_t new_len;
    char * replaced;
    zval tmp;

    // @todo this should probably support inheritance
    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1 TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    convert_to_string(val);
    replaced = php_escape_html_entities_ex(Z_STRVAL_P(val), Z_STRLEN_P(val), &new_len, 0, (int) ENT_QUOTES, "UTF-8", 1 TSRMLS_CC);
    RETURN_STRING(replaced, 0);
}
#else
static zend_always_inline void php_handlebars_escape_expression(zval * val, zval * return_value TSRMLS_DC)
{
    zend_string * replaced;
    zval tmp;
    zval rv;

    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1, &rv TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    convert_to_string(val);
    replaced = php_escape_html_entities_ex(Z_STRVAL_P(val), Z_STRLEN_P(val), 0, (int) ENT_QUOTES, "UTF-8", 1 TSRMLS_CC);
    RETURN_STR(replaced);
}
#endif

PHP_METHOD(HandlebarsNative, escapeExpression)
{
    zval * val;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }

    php_handlebars_escape_expression(val, return_value TSRMLS_CC);
}

/* }}} Handlebars\Native::escapeExpression */
/* {{{ proto string Handlebars\Native::escapeExpressionCompat(mixed value) */

static zend_always_inline char * php_handlebars_escape_expression_replace_helper(char * input TSRMLS_DC)
{
    char * output;
    char * source;
    char * target;
    char c;
    long occurrences = 0;
    long newlen;
    long oldlen = strlen(input);
    
    if( !oldlen ) {
        return estrdup(input);
    }
    
    // Count the ocurrences of ` and '
    for( source = input; (c = *source); source++ ) {
        switch( c ) {
            case '`':
            case '\'':
                occurrences++;
                break;
        }
    }
    
    if( !occurrences ) {
        return estrdup(input);
    }
    
    // Each occurence increases length by 5
    newlen = oldlen + (occurrences * 5) + 1;
    
    // Allocate new string
    output = target = (char *) emalloc(sizeof(char) * newlen);
    
    // Replace copy
    for( source = input; (c = *source); source++ ) {
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
    
    return output;
}

#if PHP_MAJOR_VERSION < 7
static zend_always_inline void php_handlebars_escape_expression_compat(zval * val, zval * return_value TSRMLS_DC)
{
    size_t new_len;
    char * replaced;
    char * replaced2;
    zval tmp;

    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1 TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    INIT_ZVAL(tmp);
    if( !php_handlebars_expression(val, &tmp TSRMLS_CC) ) {
        return;
    }

    replaced = php_escape_html_entities_ex(Z_STRVAL(tmp), Z_STRLEN(tmp), &new_len, 0, (int) ENT_COMPAT, "UTF-8", 1 TSRMLS_CC);
    zval_dtor(&tmp);

    replaced2 = php_handlebars_escape_expression_replace_helper(replaced TSRMLS_CC);
    efree(replaced);

    RETVAL_STRING(replaced2, 0);
}
#else
static zend_always_inline void php_handlebars_escape_expression_compat(zval * val, zval * return_value TSRMLS_DC)
{
    zend_string * replaced;
    char * replaced2;
    zval tmp;
    zval rv;

    // @todo this should probably support inheritance
    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1, &rv TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    ZVAL_NULL(&tmp);
    if( !php_handlebars_expression(val, &tmp TSRMLS_CC) ) {
        return;
    }

    replaced = php_escape_html_entities_ex(Z_STRVAL(tmp), Z_STRLEN(tmp), 0, (int) ENT_COMPAT, "UTF-8", 1 TSRMLS_CC);
    zval_dtor(&tmp);

    replaced2 = php_handlebars_escape_expression_replace_helper(replaced->val TSRMLS_CC);
    zend_string_free(replaced);

    RETVAL_STRING(replaced2);
    efree(replaced2);
}
#endif

PHP_METHOD(HandlebarsNative, escapeExpressionCompat)
{
    zval * val;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }

    php_handlebars_escape_expression_compat(val, return_value TSRMLS_CC);
}

/* }}} Handlebars\Native::escapeExpressionCompat */
/* {{{ proto Handlebars\SafeString::__construct(string value) */

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

/* }}} Handlebars\SafeString::__construct */
/* {{{ proto string Handlebars\SafeString::__toString() */

PHP_METHOD(HandlebarsSafeString, __toString)
{
    zval * _this_zval;
    zval * value;
    zval rv;

    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, HandlebarsSafeString_ce_ptr) == FAILURE) {
        return;
    }

#if PHP_MAJOR_VERSION < 7
    value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, 1 TSRMLS_CC);
#else
    value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, 1, &rv TSRMLS_CC);
#endif
    RETURN_ZVAL(value, 1, 0);
}

/* }}} HandlebarsSafeString::__toString */
/* {{{ Argument Info -------------------------------------------------------- */

ZEND_BEGIN_ARG_INFO_EX(HandlebarsNative_getLastError_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsNative_lex_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsNative_parse_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsNative_compile_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
    ZEND_ARG_INFO(0, flags)
    ZEND_ARG_ARRAY_INFO(0, knownHelpers, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsNative_nameLookup_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, objOrArray)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsNative_isIntArray_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, arr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsNative_expression_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsSafeString_construct_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsSafeString_toString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

/* }}} ---------------------------------------------------------------------- */
/* {{{ Method Entry --------------------------------------------------------- */

static zend_function_entry HandlebarsNative_methods[] = {
    PHP_ME(HandlebarsNative, getLastError, HandlebarsNative_getLastError_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, lex, HandlebarsNative_lex_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, lexPrint, HandlebarsNative_lex_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, parse, HandlebarsNative_parse_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, parsePrint, HandlebarsNative_parse_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, compile, HandlebarsNative_compile_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, compilePrint, HandlebarsNative_compile_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, nameLookup, HandlebarsNative_nameLookup_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, isIntArray, HandlebarsNative_isIntArray_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, expression, HandlebarsNative_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, escapeExpression, HandlebarsNative_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsNative, escapeExpressionCompat, HandlebarsNative_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  { NULL, NULL, NULL }
};

static zend_function_entry HandlebarsSafeString_methods[] = {
    PHP_ME(HandlebarsSafeString, __construct, HandlebarsSafeString_construct_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsSafeString, __toString, HandlebarsSafeString_toString_args, ZEND_ACC_PUBLIC)
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
    const char * version = handlebars_version_string();
    
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_NONE", handlebars_compiler_flag_none, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_USE_DEPTHS", handlebars_compiler_flag_use_depths, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_STRING_PARAMS", handlebars_compiler_flag_string_params, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_TRACK_IDS", handlebars_compiler_flag_track_ids, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_NO_ESCAPE", handlebars_compiler_flag_no_escape, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_KNOWN_HELPERS_ONLY", handlebars_compiler_flag_known_helpers_only, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_PREVENT_INDENT", handlebars_compiler_flag_prevent_indent, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_COMPAT", handlebars_compiler_flag_compat, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_FLAG_ALL", handlebars_compiler_flag_all, flags);

    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_RESULT_FLAG_USE_PARTIAL", handlebars_compiler_flag_use_partial, flags);
    REGISTER_LONG_CONSTANT("Handlebars\\COMPILER_RESULT_FLAG_IS_SIMPLE", handlebars_compiler_flag_is_simple, flags);

    // Handlebars\Native
    INIT_CLASS_ENTRY(ce, "Handlebars\\Native", HandlebarsNative_methods);
    HandlebarsNative_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_class_constant_stringl(HandlebarsNative_ce_ptr, ZEND_STRL("LIBVERSION"), version, strlen(version) TSRMLS_CC);
    zend_declare_class_constant_stringl(HandlebarsNative_ce_ptr, ZEND_STRL("VERSION"), PHP_HANDLEBARS_VERSION, sizeof(PHP_HANDLEBARS_VERSION)-1 TSRMLS_CC);
    
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
    NULL,                               /* Functions */
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
