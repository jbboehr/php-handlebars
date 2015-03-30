
/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <talloc.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
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

/* {{{ Globals -------------------------------------------------------------- */

ZEND_DECLARE_MODULE_GLOBALS(handlebars)

/* }}} ---------------------------------------------------------------------- */
/* {{{ Utils----------------------------------------------------------------- */

static zval * php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node TSRMLS_DC);

static void php_handlebars_error(char * msg TSRMLS_DC)
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

static zval * php_handlebars_ast_list_to_zval(struct handlebars_ast_list * list TSRMLS_DC)
{
    struct handlebars_ast_list_item * item;
    struct handlebars_ast_list_item * tmp;
    zval * current;
    
    ALLOC_INIT_ZVAL(current);
    array_init(current);
    
    if( list == NULL ) {
        return current;
    }
    
    handlebars_ast_list_foreach(list, item, tmp) {
        add_next_index_zval(current, php_handlebars_ast_node_to_zval(item->data TSRMLS_CC));
    }
    
    return current;
}

static zval * php_handlebars_ast_node_to_zval(struct handlebars_ast_node * node TSRMLS_DC)
{
    zval * current = NULL;
    char * typeName = NULL;
    
    ALLOC_INIT_ZVAL(current);
    array_init(current);
    
    if( node == NULL ) {
        // meh
        return current;
    }
    
    typeName = estrdup(handlebars_ast_node_readable_type(node->type));
    add_assoc_string_ex(current, ZEND_STRS("type"), typeName, 0);
    
    if( node->strip ) {
        zval * strip;
        ALLOC_INIT_ZVAL(strip);
        array_init(strip);
        add_assoc_bool_ex(strip, ZEND_STRS("left"), 1 && (node->strip & handlebars_ast_strip_flag_left));
        add_assoc_bool_ex(strip, ZEND_STRS("right"), 1 && (node->strip & handlebars_ast_strip_flag_right));
        add_assoc_bool_ex(strip, ZEND_STRS("openStandalone"), 1 && (node->strip & handlebars_ast_strip_flag_open_standalone));
        add_assoc_bool_ex(strip, ZEND_STRS("closeStandalone"), 1 && (node->strip & handlebars_ast_strip_flag_close_standalone));
        add_assoc_bool_ex(strip, ZEND_STRS("inlineStandalone"), 1 && (node->strip & handlebars_ast_strip_flag_inline_standalone));
        add_assoc_bool_ex(strip, ZEND_STRS("leftStripped"), 1 && (node->strip & handlebars_ast_strip_flag_left_stripped));
        add_assoc_bool_ex(strip, ZEND_STRS("rightStriped"), 1 && (node->strip & handlebars_ast_strip_flag_right_stripped));
        add_assoc_zval_ex(current, ZEND_STRS("strip"), strip);   
    }
    
    switch( node->type ) {
        case HANDLEBARS_AST_NODE_PROGRAM: {
            if( node->node.program.statements ) {
                add_assoc_zval_ex(current, ZEND_STRS("statements"),
                        php_handlebars_ast_list_to_zval(node->node.program.statements TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_MUSTACHE: {
            if( node->node.mustache.sexpr ) {
                add_assoc_zval_ex(current, ZEND_STRS("sexpr"),
                    php_handlebars_ast_node_to_zval(node->node.mustache.sexpr TSRMLS_CC));
            }
            add_assoc_long_ex(current, ZEND_STRS("unescaped"),
                    node->node.mustache.unescaped);
            break;
        }
        case HANDLEBARS_AST_NODE_SEXPR: {
            if( node->node.sexpr.hash ) {
                add_assoc_zval_ex(current, ZEND_STRS("hash"),
                        php_handlebars_ast_node_to_zval(node->node.sexpr.hash TSRMLS_CC));
            }
            if( node->node.sexpr.id ) {
                add_assoc_zval_ex(current, ZEND_STRS("id"),
                        php_handlebars_ast_node_to_zval(node->node.sexpr.id TSRMLS_CC));
            }
            if( node->node.sexpr.params ) {
                add_assoc_zval_ex(current, ZEND_STRS("params"),
                        php_handlebars_ast_list_to_zval(node->node.sexpr.params TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL:
            if( node->node.partial.partial_name ) {
                add_assoc_zval_ex(current, ZEND_STRS("partial_name"),
                        php_handlebars_ast_node_to_zval(node->node.partial.partial_name TSRMLS_CC));
            }
            if( node->node.partial.context ) {
                add_assoc_zval_ex(current, ZEND_STRS("context"),
                        php_handlebars_ast_node_to_zval(node->node.partial.context TSRMLS_CC));
            }
            if( node->node.partial.hash ) {
                add_assoc_zval_ex(current, ZEND_STRS("hash"),
                        php_handlebars_ast_node_to_zval(node->node.partial.hash TSRMLS_CC));
            }
            break;
        case HANDLEBARS_AST_NODE_RAW_BLOCK: {
            if( node->node.raw_block.mustache ) {
                add_assoc_zval_ex(current, ZEND_STRS("mustache"),
                        php_handlebars_ast_node_to_zval(node->node.raw_block.mustache TSRMLS_CC));
            }
            if( node->node.raw_block.program ) {
                add_assoc_zval_ex(current, ZEND_STRS("program"),
                        php_handlebars_ast_node_to_zval(node->node.raw_block.program TSRMLS_CC));
            }
            if( node->node.raw_block.close ) {
                add_assoc_string_ex(current, ZEND_STRS("close"),
                        node->node.raw_block.close, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_BLOCK: {
            if( node->node.block.mustache ) {
                add_assoc_zval_ex(current, ZEND_STRS("mustache"),
                        php_handlebars_ast_node_to_zval(node->node.block.mustache TSRMLS_CC));
            }
            if( node->node.block.program ) {
                add_assoc_zval_ex(current, ZEND_STRS("program"),
                        php_handlebars_ast_node_to_zval(node->node.block.program TSRMLS_CC));
            }
            if( node->node.block.inverse ) {
                add_assoc_zval_ex(current, ZEND_STRS("inverse"),
                        php_handlebars_ast_node_to_zval(node->node.block.inverse TSRMLS_CC));
            }
            if( node->node.block.close ) {
                add_assoc_zval_ex(current, ZEND_STRS("close"),
                        php_handlebars_ast_node_to_zval(node->node.block.close TSRMLS_CC));
            }
            add_assoc_long_ex(current, ZEND_STRS("inverted"), node->node.block.inverted);
            break;
        }
        case HANDLEBARS_AST_NODE_CONTENT: {
            if( node->node.content.string ) {
                add_assoc_stringl_ex(current, ZEND_STRS("string"),
                    node->node.content.string,
                    node->node.content.length, 1);
            }
            if( node->node.content.original ) {
                add_assoc_stringl_ex(current, ZEND_STRS("original"),
                    node->node.content.original,
                    strlen(node->node.content.original), 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_HASH: {
            if( node->node.hash.segments ) {
                add_assoc_zval_ex(current, ZEND_STRS("segments"),
                        php_handlebars_ast_list_to_zval(node->node.hash.segments TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_HASH_SEGMENT: {
            if( node->node.hash_segment.key ) {
                add_assoc_stringl_ex(current, ZEND_STRS("key"),
                    node->node.hash_segment.key,
                    node->node.hash_segment.key_length, 1);
            }
            if( node->node.hash_segment.value ) {
                add_assoc_zval_ex(current, ZEND_STRS("value"),
                        php_handlebars_ast_node_to_zval(node->node.hash_segment.value TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_ID: {
            if( node->node.id.parts ) {
                add_assoc_zval_ex(current, ZEND_STRS("parts"),
                        php_handlebars_ast_list_to_zval(node->node.id.parts TSRMLS_CC));
            }
            add_assoc_long_ex(current, ZEND_STRS("depth"), node->node.id.depth);
            add_assoc_long_ex(current, ZEND_STRS("is_simple"), node->node.id.is_simple);
            add_assoc_long_ex(current, ZEND_STRS("is_scoped"), node->node.id.is_scoped);
            if( node->node.id.id_name ) {
                add_assoc_stringl_ex(current, ZEND_STRS("id_name"),
                    node->node.id.id_name,
                    node->node.id.id_name_length, 1);
            }
            if( node->node.id.string ) {
                add_assoc_stringl_ex(current, ZEND_STRS("string"),
                    node->node.id.string,
                    node->node.id.string_length, 1);
            }
            if( node->node.id.original ) {
                add_assoc_stringl_ex(current, ZEND_STRS("original"),
                    node->node.id.original,
                    node->node.id.original_length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL_NAME: {
            if( node->node.partial_name.name ) {
                add_assoc_zval_ex(current, ZEND_STRS("name"),
                        php_handlebars_ast_node_to_zval(node->node.partial_name.name TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_DATA: {
            if( node->node.data.id ) {
                add_assoc_zval_ex(current, ZEND_STRS("id"),
                        php_handlebars_ast_node_to_zval(node->node.data.id TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_STRING: {
            if( node->node.string.string ) {
                add_assoc_stringl_ex(current, ZEND_STRS("string"),
                    node->node.string.string,
                    node->node.string.length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_NUMBER: {
            if( node->node.number.string ) {
                add_assoc_stringl_ex(current, ZEND_STRS("number"),
                    node->node.number.string,
                    node->node.number.length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_BOOLEAN: {
            if( node->node.boolean.string ) {
                add_assoc_stringl_ex(current, ZEND_STRS("boolean"),
                    node->node.boolean.string,
                    node->node.boolean.length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_COMMENT: {
            if( node->node.comment.comment ) {
                add_assoc_stringl_ex(current, ZEND_STRS("comment"),
                    node->node.comment.comment,
                    node->node.boolean.length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PATH_SEGMENT: {
            if( node->node.path_segment.separator ) {
                add_assoc_stringl_ex(current, ZEND_STRS("separator"),
                    node->node.path_segment.separator,
                    node->node.path_segment.separator_length, 1);
            }
            if( node->node.path_segment.part ) {
                add_assoc_stringl_ex(current, ZEND_STRS("part"),
                    node->node.path_segment.part,
                    node->node.path_segment.part_length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_NIL:
            break;
    }
    
    return current;
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
            add_next_index_string(arr, operand->data.stringval, 1);
            break;
        case handlebars_operand_type_array: {
            zval * current;
            char ** tmp = operand->data.arrayval;
            
            ALLOC_INIT_ZVAL(current);
            array_init(current);
            
            for( ; *tmp; ++tmp ) {
                add_next_index_string(current, *tmp, 1);
            }
            
            add_next_index_zval(arr, current);
            break;
        }
    }
}

static zval * php_handlebars_opcode_to_zval(struct handlebars_opcode * opcode TSRMLS_DC)
{
    zval * current;
    zval * args;
    char * name = estrdup(handlebars_opcode_readable_type(opcode->type));
    short num = handlebars_opcode_num_operands(opcode->type);
    
    ALLOC_INIT_ZVAL(current);
    array_init(current);
    
    // note: readable tpye should be a const char so STRS should work
    add_assoc_stringl_ex(current, "opcode", sizeof("opcode"), name, strlen(name), 0);
    
    ALLOC_INIT_ZVAL(args);
    array_init(args);
    
    add_assoc_zval_ex(current, "args", sizeof("args"), args);
    
    if( num >= 1 ) {
        php_handlebars_operand_append_zval(&opcode->op1, args TSRMLS_CC);
    }
    if( num >= 2 ) {
        php_handlebars_operand_append_zval(&opcode->op2, args TSRMLS_CC);
    }
    if( num >= 3 ) {
        php_handlebars_operand_append_zval(&opcode->op3, args TSRMLS_CC);
    }
    
    return current;
}

static zval * php_handlebars_opcodes_to_zval(struct handlebars_opcode ** opcodes, size_t count TSRMLS_DC)
{
    zval * current;
    size_t i;
    struct handlebars_opcode ** pos = opcodes;
    short num;
    
    ALLOC_INIT_ZVAL(current);
    array_init(current);
    
    for( i = 0; i < count; i++, pos++ ) {
        add_next_index_zval(current, php_handlebars_opcode_to_zval(*pos TSRMLS_CC));
    }
    
    return current;
}

static zval * php_handlebars_compiler_to_zval(struct handlebars_compiler * compiler TSRMLS_DC)
{
    zval * current = NULL;
    zval * children = NULL;
    zval * zdepths = NULL;
    size_t i;
    struct handlebars_compiler * child;
    long depths;
    int depthi;
    
    ALLOC_INIT_ZVAL(current);
    array_init(current);
    
    // Opcodes
    add_assoc_zval_ex(current, "opcodes", sizeof("opcodes"),
            php_handlebars_opcodes_to_zval(compiler->opcodes, compiler->opcodes_length TSRMLS_CC));
    
    // Children
    ALLOC_INIT_ZVAL(children);
    array_init(children);
    
    for( i = 0; i < compiler->children_length; i++ ) {
        child = *(compiler->children + i);
        add_next_index_zval(children, php_handlebars_compiler_to_zval(child TSRMLS_CC));
    }
    
    add_assoc_zval_ex(current, "children", sizeof("children"), children);
    
    // Depths
    ALLOC_INIT_ZVAL(zdepths);
    array_init(zdepths);
    
    depths = compiler->depths;
    depthi = 1;
    while( depths > 0 ) {
        if( depths & 1 ) {
            add_next_index_long(zdepths, depthi);
        }
        depthi++;
        depths = depths >> 1;
    }
    
    add_assoc_zval_ex(current, "depths", sizeof("depths"), zdepths);
    
    // Return
    return current;
}

static char ** php_handlebars_compiler_known_helpers_from_zval(void * ctx, zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    HashPosition data_pointer = NULL;
    zval ** data_entry = NULL;
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
    zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
    while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
        if( Z_TYPE_PP(data_entry) == IS_STRING ) {
            *ptr++ = (char *) handlebars_talloc_strdup(ctx, Z_STRVAL_PP(data_entry));
        }
        zend_hash_move_forward_ex(data_hash, &data_pointer);
    }

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

PHP_FUNCTION(handlebars_error)
{
    if( HANDLEBARS_G(handlebars_last_error) ) {
        RETURN_STRING(HANDLEBARS_G(handlebars_last_error), 1);
    }
}

PHP_FUNCTION(handlebars_lex)
{
    char * tmpl;
    long tmpl_len;
    struct handlebars_context * ctx;
    struct handlebars_token_list * list;
    struct handlebars_token_list_item * el = NULL;
    struct handlebars_token_list_item * tmp = NULL;
    struct handlebars_token * token = NULL;
    char * name;
    zval * child;
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        RETURN_FALSE;
    }
    
    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    list = handlebars_lex(ctx);
    
    array_init(return_value);
    
    handlebars_token_list_foreach(list, el, tmp) {
        token = el->data;
        name = estrdup(handlebars_token_readable_type(token->token));
        
        child = NULL;
        ALLOC_INIT_ZVAL(child);
        array_init(child);
        add_assoc_string_ex(child, "name", sizeof("name"), name, 0);
        if( token->text ) {
            add_assoc_string_ex(child, "text", sizeof("text"), token->text, 1);
        }
        add_next_index_zval(return_value, child);
    }
    
    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_lex_print)
{
    char * tmpl;
    long tmpl_len;
    struct handlebars_context * ctx;
    struct handlebars_token_list * list;
    char * output;
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        RETURN_FALSE;
    }
    
    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    list = handlebars_lex(ctx);
    output = handlebars_token_list_print(list, 0);
    
    RETVAL_STRING(output, 1);
    
    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_parse)
{
    char * tmpl;
    long tmpl_len;
    struct handlebars_context * ctx;
    int retval;
    zval * output;
    char * errmsg;
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        RETURN_FALSE;
    }
    
    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    retval = handlebars_yy_parse(ctx);
    
    if( ctx->error != NULL ) {
        // errmsg will be freed by the destruction of ctx
        errmsg = handlebars_context_get_errmsg(ctx);
        php_handlebars_error(errmsg TSRMLS_CC);
        RETVAL_FALSE;
    } else {
        output = php_handlebars_ast_node_to_zval(ctx->program TSRMLS_CC);
        RETVAL_ZVAL(output, 0, 0);
    }
    
    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_parse_print)
{
    char * tmpl;
    long tmpl_len;
    struct handlebars_context * ctx;
    int retval;
    char * output;
    char * errmsg;
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        RETURN_FALSE;
    }
    
    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    retval = handlebars_yy_parse(ctx);
    
    if( ctx->error != NULL ) {
        // errmsg will be freed by the destruction of ctx
        errmsg = handlebars_context_get_errmsg(ctx);
        php_handlebars_error(errmsg TSRMLS_CC);
        RETVAL_FALSE;
    } else {
        output = handlebars_ast_print(ctx->program, 0);
        RETVAL_STRING(output, 1);
    }
    
    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_compile)
{
    char * tmpl;
    long tmpl_len;
    long flags = 0;
    zval * known_helpers = NULL;
    struct handlebars_context * ctx;
    struct handlebars_compiler * compiler;
    struct handlebars_opcode_printer * printer;
    int retval;
    zval * output;
    char * errmsg;
    char ** known_helpers_arr;
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz", &tmpl, &tmpl_len, &flags, &known_helpers) == FAILURE ) {
        RETURN_FALSE;
    }
    
    // Initialize
    ctx = handlebars_context_ctor();
    compiler = handlebars_compiler_ctor(ctx);
    printer = handlebars_opcode_printer_ctor(ctx);
    handlebars_compiler_set_flags(compiler, flags);
    
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
        php_handlebars_error(errmsg TSRMLS_CC);
        RETVAL_FALSE;
        goto error;
    }
    
    // Compile
    handlebars_compiler_compile(compiler, ctx->program);
    if( compiler->errnum ) {
        // @todo decent error message
        RETVAL_FALSE;
        goto error;
    }
    
    output = php_handlebars_compiler_to_zval(compiler TSRMLS_CC);
    RETVAL_ZVAL(output, 0, 0);
    
error:
    handlebars_context_dtor(ctx);
}

PHP_FUNCTION(handlebars_compile_print)
{
    char * tmpl;
    long tmpl_len;
    long flags = 0;
    zval * known_helpers = NULL;
    struct handlebars_context * ctx;
    struct handlebars_compiler * compiler;
    struct handlebars_opcode_printer * printer;
    int retval;
    char * output;
    char * errmsg;
    char ** known_helpers_arr;
    
    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz", &tmpl, &tmpl_len, &flags, &known_helpers) == FAILURE ) {
        RETURN_FALSE;
    }
    
    // Initialize
    ctx = handlebars_context_ctor();
    compiler = handlebars_compiler_ctor(ctx);
    printer = handlebars_opcode_printer_ctor(ctx);
    handlebars_compiler_set_flags(compiler, flags);
    
    // Get known helpers
    known_helpers_arr = php_handlebars_compiler_known_helpers_from_zval(ctx, known_helpers TSRMLS_CC);
    if( known_helpers_arr ) {
        compiler->known_helpers = (const char **) known_helpers_arr;
    }
    
    // Parse
    ctx->tmpl = tmpl;
    retval = handlebars_yy_parse(ctx);
    
    if( ctx->error != NULL ) {
        // errmsg will be freed by the destruction of ctx
        errmsg = handlebars_context_get_errmsg(ctx);
        php_handlebars_error(errmsg TSRMLS_CC);
        RETVAL_FALSE;
        goto error;
    }
    
    // Compile
    handlebars_compiler_compile(compiler, ctx->program);
    if( compiler->errnum ) {
        // @todo decent error message
        RETVAL_FALSE;
        goto error;
    }
    
    // Printer
    handlebars_opcode_printer_print(printer, compiler);
    RETVAL_STRING(printer->output, 1);
    
error:
    handlebars_context_dtor(ctx);
}


PHP_FUNCTION(handlebars_version)
{
    RETURN_STRING(handlebars_version_string(), 1);
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Hooks --------------------------------------------------------- */

static PHP_GINIT_FUNCTION(handlebars)
{
    handlebars_globals->handlebars_last_error = NULL;
}

static PHP_MINIT_FUNCTION(handlebars)
{
    int flags = CONST_CS | CONST_PERSISTENT | CONST_CT_SUBST;
    
    REGISTER_LONG_CONSTANT("HANDLEBARS_COMPILER_FLAG_NONE", handlebars_compiler_flag_none, flags);
    REGISTER_LONG_CONSTANT("HANDLEBARS_COMPILER_FLAG_USE_DEPTHS", handlebars_compiler_flag_use_depths, flags);
    REGISTER_LONG_CONSTANT("HANDLEBARS_COMPILER_FLAG_STRING_PARAMS", handlebars_compiler_flag_string_params, flags);
    REGISTER_LONG_CONSTANT("HANDLEBARS_COMPILER_FLAG_TRACK_IDS", handlebars_compiler_flag_track_ids, flags);
    REGISTER_LONG_CONSTANT("HANDLEBARS_COMPILER_FLAG_KNOWN_HELPERS_ONLY", handlebars_compiler_flag_known_helpers_only, flags);
    REGISTER_LONG_CONSTANT("HANDLEBARS_COMPILER_FLAG_COMPAT", handlebars_compiler_flag_compat, flags);
    REGISTER_LONG_CONSTANT("HANDLEBARS_COMPILER_FLAG_ALL", handlebars_compiler_flag_all, flags);
    
    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(handlebars)
{
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
    PHP_FE_END
};

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Entry --------------------------------------------------------- */

zend_module_entry handlebars_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_HANDLEBARS_NAME,                /* Name */
    handlebars_functions,               /* Functions */
    PHP_MINIT(handlebars),              /* MINIT */
    PHP_MSHUTDOWN(handlebars),          /* MSHUTDOWN */
    NULL,                               /* RINIT */
    NULL,                               /* RSHUTDOWN */
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
