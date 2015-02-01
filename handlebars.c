
/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
#include "handlebars_context.h"
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
	char * name = NULL;
	
	ALLOC_INIT_ZVAL(current);
	array_init(current);
	
	if( node == NULL ) {
	    // meh
	    return current;
	}
	
	name = estrdup(handlebars_ast_node_readable_type(node->type));
	add_assoc_string_ex(current, "name", sizeof("name"), name, 0);
	
    switch( node->type ) {
        case HANDLEBARS_AST_NODE_PROGRAM: {
            if( node->node.program.statements ) {
    			add_assoc_zval_ex(current, "statements", sizeof("statements"),
    					php_handlebars_ast_list_to_zval(node->node.program.statements TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_MUSTACHE: {
            if( node->node.mustache.sexpr ) {
                add_assoc_zval_ex(current, "sexpr", sizeof("sexpr"),
					php_handlebars_ast_node_to_zval(node->node.mustache.sexpr TSRMLS_CC));
            }
			add_assoc_bool_ex(current, "escaped", sizeof("escaped"),
					node->node.mustache.escaped);
            break;
        }
        case HANDLEBARS_AST_NODE_SEXPR: {
			if( node->node.sexpr.hash ) {
				add_assoc_zval_ex(current, "hash", sizeof("hash"),
						php_handlebars_ast_node_to_zval(node->node.sexpr.hash TSRMLS_CC));
			}
			if( node->node.sexpr.id ) {
				add_assoc_zval_ex(current, "id", sizeof("id"),
						php_handlebars_ast_node_to_zval(node->node.sexpr.id TSRMLS_CC));
			}
			if( node->node.sexpr.params ) {
				add_assoc_zval_ex(current, "params", sizeof("params"),
						php_handlebars_ast_list_to_zval(node->node.sexpr.params TSRMLS_CC));
			}
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL:
			if( node->node.partial.partial_name ) {
				add_assoc_zval_ex(current, "partial_name", sizeof("partial_name"),
						php_handlebars_ast_node_to_zval(node->node.partial.partial_name TSRMLS_CC));
			}
			if( node->node.partial.context ) {
				add_assoc_zval_ex(current, "context", sizeof("context"),
						php_handlebars_ast_node_to_zval(node->node.partial.context TSRMLS_CC));
			}
			if( node->node.partial.hash ) {
				add_assoc_zval_ex(current, "hash", sizeof("hash"),
						php_handlebars_ast_node_to_zval(node->node.partial.hash TSRMLS_CC));
			}
            break;
        case HANDLEBARS_AST_NODE_RAW_BLOCK: {
			if( node->node.raw_block.mustache ) {
				add_assoc_zval_ex(current, "mustache", sizeof("mustache"),
						php_handlebars_ast_node_to_zval(node->node.raw_block.mustache TSRMLS_CC));
			}
			if( node->node.raw_block.program ) {
				add_assoc_zval_ex(current, "program", sizeof("program"),
						php_handlebars_ast_node_to_zval(node->node.raw_block.program TSRMLS_CC));
			}
			if( node->node.raw_block.close ) {
			    add_assoc_string_ex(current, "close", sizeof("close"),
						node->node.raw_block.close, 1 TSRMLS_CC);
			}
            break;
        }
        case HANDLEBARS_AST_NODE_BLOCK: {
			if( node->node.block.mustache ) {
				add_assoc_zval_ex(current, "mustache", sizeof("mustache"),
						php_handlebars_ast_node_to_zval(node->node.block.mustache TSRMLS_CC));
			}
			if( node->node.block.program ) {
				add_assoc_zval_ex(current, "program", sizeof("program"),
						php_handlebars_ast_node_to_zval(node->node.block.program TSRMLS_CC));
			}
			if( node->node.block.inverse ) {
				add_assoc_zval_ex(current, "inverse", sizeof("inverse"),
						php_handlebars_ast_node_to_zval(node->node.block.inverse TSRMLS_CC));
			}
			if( node->node.block.close ) {
				add_assoc_zval_ex(current, "close", sizeof("close"),
						php_handlebars_ast_node_to_zval(node->node.block.close TSRMLS_CC));
			}
			add_assoc_bool_ex(current, "inverted", sizeof("inverted"), node->node.block.inverted);
            break;
        }
        case HANDLEBARS_AST_NODE_CONTENT: {
            if( node->node.content.string ) {
                add_assoc_stringl_ex(current, "string", sizeof("string"),
                    node->node.content.string,
                    node->node.content.length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_HASH: {
            if( node->node.hash.segments ) {
    			add_assoc_zval_ex(current, "segments", sizeof("segments"),
    					php_handlebars_ast_list_to_zval(node->node.hash.segments TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_HASH_SEGMENT: {
            if( node->node.hash_segment.key ) {
                add_assoc_stringl_ex(current, "key", sizeof("key"),
                    node->node.hash_segment.key,
                    node->node.hash_segment.key_length, 1);
            }
            if( node->node.hash_segment.value ) {
				add_assoc_zval_ex(current, "value", sizeof("value"),
						php_handlebars_ast_node_to_zval(node->node.hash_segment.value TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_ID: {
			if( node->node.id.parts ) {
				add_assoc_zval_ex(current, "parts", sizeof("parts"),
						php_handlebars_ast_list_to_zval(node->node.id.parts TSRMLS_CC));
			}
			add_assoc_long_ex(current, "depth", sizeof("depth"), node->node.id.depth);
			add_assoc_bool_ex(current, "is_simple", sizeof("inverted"), node->node.id.is_simple);
			add_assoc_bool_ex(current, "is_scoped", sizeof("inverted"), node->node.id.is_scoped);
            if( node->node.id.id_name ) {
                add_assoc_stringl_ex(current, "id_name", sizeof("id_name"),
                    node->node.id.id_name,
                    node->node.id.id_name_length, 1);
            }
            if( node->node.id.string ) {
                add_assoc_stringl_ex(current, "string", sizeof("string"),
                    node->node.id.string,
                    node->node.id.string_length, 1);
            }
            if( node->node.id.original ) {
                add_assoc_stringl_ex(current, "original", sizeof("original"),
                    node->node.id.original,
                    node->node.id.original_length, 1);
            }
            break;
        }
        case HANDLEBARS_AST_NODE_PARTIAL_NAME: {
            if( node->node.partial_name.name ) {
				add_assoc_zval_ex(current, "name", sizeof("name"),
						php_handlebars_ast_node_to_zval(node->node.partial_name.name TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_DATA: {
            if( node->node.data.id ) {
				add_assoc_zval_ex(current, "id", sizeof("id"),
						php_handlebars_ast_node_to_zval(node->node.data.id TSRMLS_CC));
            }
            break;
        }
        case HANDLEBARS_AST_NODE_STRING: {
            if( node->node.string.string ) {
                add_assoc_stringl_ex(current, "string", sizeof("string"),
                    node->node.string.string,
                    node->node.string.length, 1);
            }
        }
        case HANDLEBARS_AST_NODE_NUMBER: {
            if( node->node.number.string ) {
                add_assoc_stringl_ex(current, "number", sizeof("number"),
                    node->node.number.string,
                    node->node.number.length, 1);
            }
        }
        case HANDLEBARS_AST_NODE_BOOLEAN: {
            if( node->node.boolean.string ) {
                add_assoc_stringl_ex(current, "boolean", sizeof("boolean"),
                    node->node.boolean.string,
                    node->node.boolean.length, 1);
            }
        }
        case HANDLEBARS_AST_NODE_COMMENT: {
            if( node->node.comment.comment ) {
                add_assoc_stringl_ex(current, "comment", sizeof("comment"),
                    node->node.comment.comment,
                    node->node.boolean.length, 1);
            }
        }
        case HANDLEBARS_AST_NODE_PATH_SEGMENT:
            if( node->node.path_segment.separator ) {
                add_assoc_stringl_ex(current, "separator", sizeof("separator"),
                    node->node.path_segment.separator,
                    node->node.path_segment.separator_length, 1);
            }
            if( node->node.path_segment.part ) {
                add_assoc_stringl_ex(current, "part", sizeof("part"),
                    node->node.path_segment.part,
                    node->node.path_segment.part_length, 1);
            }
            break;
        case HANDLEBARS_AST_NODE_NIL:
            break;
    }
	
	return current;
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

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Hooks --------------------------------------------------------- */

static PHP_MINIT_FUNCTION(handlebars)
{
    HANDLEBARS_G(handlebars_last_error) = NULL;
    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(handlebars)
{
    php_handlebars_error(NULL TSRMLS_CC);
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
/* {{{ Function Entry ------------------------------------------------------- */

zend_function_entry handlebars_functions[] = {
    PHP_FE(handlebars_error, NULL)
    PHP_FE(handlebars_lex, NULL)
    PHP_FE(handlebars_lex_print, NULL)
    PHP_FE(handlebars_parse, NULL)
    PHP_FE(handlebars_parse_print, NULL)
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
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HANDLEBARS 
    ZEND_GET_MODULE(handlebars)      // Common for all PHP extensions which are build as shared modules  
#endif

/* }}} ---------------------------------------------------------------------- */
