
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <talloc.h>
#include <handlebars.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_ini.h"
#include "Zend/zend_modules.h"
#include "Zend/zend_operators.h"
#include "main/php.h"
#include "main/php_ini.h"
#include "ext/standard/info.h"

#include "php5to7.h"
#include "php_handlebars.h"

#include "handlebars.h"
#include "handlebars_cache.h"

/* {{{ Prototypes */
extern PHP_MINIT_FUNCTION(handlebars_compiler);
extern PHP_MINIT_FUNCTION(handlebars_exceptions);
extern PHP_MINIT_FUNCTION(handlebars_impl);
extern PHP_MINIT_FUNCTION(handlebars_parser);
extern PHP_MINIT_FUNCTION(handlebars_opcode);
extern PHP_MINIT_FUNCTION(handlebars_options);
extern PHP_MINIT_FUNCTION(handlebars_program);
extern PHP_MINIT_FUNCTION(handlebars_registry);
extern PHP_MINIT_FUNCTION(handlebars_safe_string);
extern PHP_MINIT_FUNCTION(handlebars_token);
extern PHP_MINIT_FUNCTION(handlebars_tokenizer);
extern PHP_MINIT_FUNCTION(handlebars_utils);
extern PHP_MINIT_FUNCTION(handlebars_value);
extern PHP_MINIT_FUNCTION(handlebars_vm);
extern PHP_MSHUTDOWN_FUNCTION(handlebars_options);

ZEND_DECLARE_MODULE_GLOBALS(handlebars);

zend_bool handlebars_has_psr = 0;
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
    STD_PHP_INI_BOOLEAN("handlebars.cache.enabled", "1", PHP_INI_ALL, OnUpdateBool, cache_enabled, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.max_size", "52428800", PHP_INI_SYSTEM, OnUpdateLong, cache_max_size, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.max_entries", "100", PHP_INI_SYSTEM, OnUpdateLong, cache_max_entries, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.max_age", "3600", PHP_INI_SYSTEM, OnUpdateLong, cache_max_age, zend_handlebars_globals, handlebars_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(handlebars)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;
    const char * version = handlebars_version_string();
    jmp_buf buf;
    struct handlebars_cache * cache;

    REGISTER_INI_ENTRIES();

#if PHP_API_VERSION >= 20100412
    if (php5to7_zend_hash_exists(&module_registry, "psr", sizeof("psr") - 1)) {
        handlebars_has_psr = 1;
    }
#endif
    REGISTER_LONG_CONSTANT("HANDLEBARS_USE_PSR", handlebars_has_psr, CONST_CS | CONST_PERSISTENT);

    REGISTER_STRING_CONSTANT("Handlebars\\VERSION", (char *) PHP_HANDLEBARS_VERSION, flags);
    REGISTER_STRING_CONSTANT("Handlebars\\LIBVERSION", (char *) version, flags);

    HANDLEBARS_G(root) = talloc_new(NULL);
    HANDLEBARS_G(context) = handlebars_context_ctor_ex(HANDLEBARS_G(root));
    //HANDLEBARS_G(cache) = handlebars_cache_ctor(HANDLEBARS_G(context));
    //HANDLEBARS_G(cache) = handlebars_cache_lmdb_ctor(HANDLEBARS_G(context), "/tmp");
    HANDLEBARS_G(cache) = handlebars_cache_mmap_ctor(HANDLEBARS_G(context));
    HANDLEBARS_G(cache)->max_entries = HANDLEBARS_G(cache_max_entries);
    HANDLEBARS_G(cache)->max_size = HANDLEBARS_G(cache_max_size);

    PHP_MINIT(handlebars_impl)(INIT_FUNC_ARGS_PASSTHRU);

    PHP_MINIT(handlebars_compiler)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_opcode)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_options)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_parser)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_program)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_registry)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_safe_string)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_token)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_tokenizer)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_utils)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_value)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_vm)(INIT_FUNC_ARGS_PASSTHRU);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(handlebars)
{
    UNREGISTER_INI_ENTRIES();

    PHP_MSHUTDOWN(handlebars_options)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

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
    php_info_print_table_row(2, "PSR support", handlebars_has_psr ? "active" : "inactive");
    snprintf(buf, sizeof(buf), "%ld", talloc_total_size(HANDLEBARS_G(root)));
    php_info_print_table_row(2, "Memory usage", buf);
    //if( HANDLEBARS_G(cache) ) {
        snprintf(buf, sizeof(buf), "%ld", HANDLEBARS_G(cache)->current_entries);
        php_info_print_table_row(2, "Cache entries", buf);
        snprintf(buf, sizeof(buf), "%ld", HANDLEBARS_G(cache)->current_size);
        php_info_print_table_row(2, "Cache size", buf);
    //}
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(handlebars)
{
    handlebars_globals->root = NULL;
    handlebars_globals->context = NULL;
    handlebars_globals->cache = NULL;
    handlebars_globals->pool_size = -1;
    handlebars_globals->cache_enabled = 1;
    handlebars_globals->cache_max_age = 3600;
    handlebars_globals->cache_max_entries = 100;
    handlebars_globals->cache_max_size = 52428800;
}
/* }}} */

/* {{{ handlebars_deps
 */
static const zend_module_dep handlebars_deps[] = {
    ZEND_MOD_OPTIONAL("psr")
    ZEND_MOD_END
};
/* }}} */

/* {{{ Module Entry */
zend_module_entry handlebars_module_entry = {
    STANDARD_MODULE_HEADER_EX, NULL,
    handlebars_deps,                    /* Deps */
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
