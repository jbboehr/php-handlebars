
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <talloc.h>
#include <handlebars.h>

#include "php.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Prototypes */
extern PHP_MINIT_FUNCTION(handlebars_compiler);
extern PHP_MINIT_FUNCTION(handlebars_exceptions);
extern PHP_MINIT_FUNCTION(handlebars_parser);
extern PHP_MINIT_FUNCTION(handlebars_opcode);
extern PHP_MINIT_FUNCTION(handlebars_safe_string);
extern PHP_MINIT_FUNCTION(handlebars_tokenizer);
extern PHP_MINIT_FUNCTION(handlebars_utils);
/* }}} Prototypes */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(handlebars)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;
    const char * version = handlebars_version_string();

    REGISTER_STRING_CONSTANT("Handlebars\\VERSION", (char *) PHP_HANDLEBARS_VERSION, flags);
    REGISTER_STRING_CONSTANT("Handlebars\\LIBVERSION", (char *) version, flags);
    
    PHP_MINIT(handlebars_compiler)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_opcode)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_parser)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_safe_string)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_token)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_tokenizer)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_utils)(INIT_FUNC_ARGS_PASSTHRU);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
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
/* }}} */

/* {{{ Module Entry */
zend_module_entry handlebars_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_HANDLEBARS_NAME,                /* Name */
    NULL,                               /* Functions */
    PHP_MINIT(handlebars),              /* MINIT */
    NULL,                               /* MSHUTDOWN */
    NULL,                               /* RINIT */
    NULL,          						/* RSHUTDOWN */
    PHP_MINFO(handlebars),              /* MINFO */
    PHP_HANDLEBARS_VERSION,             /* Version */
    STANDARD_MODULE_PROPERTIES
};
#ifdef COMPILE_DL_HANDLEBARS 
    ZEND_GET_MODULE(handlebars)      // Common for all PHP extensions which are build as shared modules  
#endif
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
