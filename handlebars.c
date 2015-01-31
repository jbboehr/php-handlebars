
/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_handlebars.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#include "handlebars.h"
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

static void php_handlebars_error(char * msg TSRMLS_DC) {
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
/* {{{ Functions ------------------------------------------------------------ */

PHP_FUNCTION(handlebars_error)
{
    if( HANDLEBARS_G(handlebars_last_error) ) {
        RETURN_STRING(HANDLEBARS_G(handlebars_last_error), 1);
    }
}

PHP_FUNCTION(handlebars_lex)
{
    // @todo
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
    // @todo
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
        output = handlebars_ast_print(ctx->program);
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
