
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <talloc.h>
#include <handlebars.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_ini.h"
#include "Zend/zend_operators.h"
#include "main/php.h"
#include "main/php_ini.h"
#include "ext/standard/info.h"

#include "php5to7.h"
#include "php_handlebars.h"

#include "handlebars_cache.h"

/* {{{ Prototypes */
extern PHP_MINIT_FUNCTION(handlebars_compile_context);
extern PHP_MINIT_FUNCTION(handlebars_compiler);
extern PHP_MINIT_FUNCTION(handlebars_exceptions);
extern PHP_MINIT_FUNCTION(handlebars_parser);
extern PHP_MINIT_FUNCTION(handlebars_opcode);
extern PHP_MINIT_FUNCTION(handlebars_options);
extern PHP_MINIT_FUNCTION(handlebars_registry);
extern PHP_MINIT_FUNCTION(handlebars_safe_string);
extern PHP_MINIT_FUNCTION(handlebars_token);
extern PHP_MINIT_FUNCTION(handlebars_tokenizer);
extern PHP_MINIT_FUNCTION(handlebars_utils);
extern PHP_MINIT_FUNCTION(handlebars_vm);

ZEND_DECLARE_MODULE_GLOBALS(handlebars);
/* }}} Prototypes */

/* {{{ PHP_INI_MH */
static PHP_INI_MH(OnUpdatePoolSize)
{
#ifdef ZEND_ENGINE_3
    zend_long s = zend_atol(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
#else
    zend_long s = zend_atol(new_value, strlen(new_value));
#endif
    HANDLEBARS_G(pool_size) = s;

    return SUCCESS;
}
/* }}} */

/* {{{ php.ini directive registration */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("handlebars.pool_size", "-1", PHP_INI_ALL, OnUpdatePoolSize, pool_size, zend_handlebars_globals, handlebars_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(handlebars)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;
    const char * version = handlebars_version_string();
    jmp_buf buf;

    REGISTER_INI_ENTRIES();

    REGISTER_STRING_CONSTANT("Handlebars\\VERSION", (char *) PHP_HANDLEBARS_VERSION, flags);
    REGISTER_STRING_CONSTANT("Handlebars\\LIBVERSION", (char *) version, flags);

    HANDLEBARS_G(root) = talloc_new(NULL);
    HANDLEBARS_G(context) = handlebars_context_ctor_ex(HANDLEBARS_G(root));

    if( handlebars_setjmp_ex(HANDLEBARS_G(context), &buf) ) {
        // @todo log?
        return FAILURE;
    }
    HANDLEBARS_G(cache) = handlebars_cache_ctor(HANDLEBARS_G(context));

    PHP_MINIT(handlebars_compile_context)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_compiler)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_opcode)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_options)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_parser)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_registry)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_safe_string)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_token)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_tokenizer)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_utils)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_vm)(INIT_FUNC_ARGS_PASSTHRU);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(handlebars)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(handlebars)
{
    char buf[64];
    php_info_print_table_start();
    php_info_print_table_row(2, "Version", PHP_HANDLEBARS_VERSION);
    php_info_print_table_row(2, "Released", PHP_HANDLEBARS_RELEASE);
    php_info_print_table_row(2, "Authors", PHP_HANDLEBARS_AUTHORS);
    // @todo make spec version from libhandlebars function
    php_info_print_table_row(2, "Spec Version", PHP_HANDLEBARS_SPEC);
    php_info_print_table_row(2, "libhandlebars Version", handlebars_version_string());
    php_info_print_table_row(2, "libhandlebars Handlebars Spec Version", handlebars_spec_version_string());
    php_info_print_table_row(2, "libhandlebars Mustache Spec Version", handlebars_mustache_spec_version_string());
    snprintf(buf, sizeof(buf), "%ld", HANDLEBARS_G(cache)->current_entries);
    php_info_print_table_row(2, "Cache entries", buf);
    snprintf(buf, sizeof(buf), "%ld", HANDLEBARS_G(cache)->current_size);
    php_info_print_table_row(2, "Cache size", buf);
    snprintf(buf, sizeof(buf), "%ld", talloc_total_size(HANDLEBARS_G(root)));
    php_info_print_table_row(2, "Memory usage", buf);
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(handlebars)
{
    handlebars_globals->root = NULL;
    handlebars_globals->pool_size = -1;
    memset(&handlebars_globals->cache, 0, sizeof(handlebars_globals->cache));
}
/* }}} */

/* {{{ Module Entry */
zend_module_entry handlebars_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_HANDLEBARS_NAME,                /* Name */
    NULL,                               /* Functions */
    PHP_MINIT(handlebars),              /* MINIT */
    PHP_MSHUTDOWN(handlebars),          /* MSHUTDOWN */
    NULL,                               /* RINIT */
    NULL,          						/* RSHUTDOWN */
    PHP_MINFO(handlebars),              /* MINFO */
    PHP_HANDLEBARS_VERSION,             /* Version */
    PHP_MODULE_GLOBALS(handlebars),     /* Globals */
    PHP_GINIT(handlebars),              /* GINIT */
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
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
