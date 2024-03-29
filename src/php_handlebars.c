
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <setjmp.h>
#include <talloc.h>
#include <handlebars.h>
#include <string.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_ini.h"
#include "Zend/zend_modules.h"
#include "Zend/zend_operators.h"
#include "main/php.h"
#include "main/php_ini.h"
#include "main/SAPI.h"
#include "ext/standard/info.h"

#include "php_handlebars.h"

#include "handlebars_cache.h"
#include "handlebars_string.h"

#pragma GCC diagnostic warning "-Wshadow"



/* {{{ Prototypes */
extern PHP_MINIT_FUNCTION(handlebars_exceptions);
extern PHP_MINIT_FUNCTION(handlebars_impl);
extern PHP_MINIT_FUNCTION(handlebars_options);
extern PHP_MINIT_FUNCTION(handlebars_registry);
extern PHP_MINIT_FUNCTION(handlebars_safe_string);
extern PHP_MINIT_FUNCTION(handlebars_utils);
extern PHP_MINIT_FUNCTION(handlebars_value);
extern PHP_MINIT_FUNCTION(handlebars_vm);
extern PHP_MSHUTDOWN_FUNCTION(handlebars_options);

#ifdef PHP_HANDLEBARS_ENABLE_AST
extern PHP_MINIT_FUNCTION(handlebars_compiler);
extern PHP_MINIT_FUNCTION(handlebars_parser);
extern PHP_MINIT_FUNCTION(handlebars_opcode);
extern PHP_MINIT_FUNCTION(handlebars_program);
extern PHP_MINIT_FUNCTION(handlebars_token);
extern PHP_MINIT_FUNCTION(handlebars_tokenizer);
#endif

ZEND_DECLARE_MODULE_GLOBALS(handlebars);

PHP_HANDLEBARS_API zend_bool handlebars_has_psr = 0;
/* }}} Prototypes */

/* {{{ php.ini directive registration */
PHP_INI_BEGIN()
    // @TODO FIXME (pool not working so set to zero)
    STD_PHP_INI_ENTRY("handlebars.pool_size", "0", PHP_INI_ALL, OnUpdateLong, pool_size, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_BOOLEAN("handlebars.cache.enable", "1", PHP_INI_SYSTEM, OnUpdateBool, cache_enable, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_BOOLEAN("handlebars.cache.enable_cli", "0", PHP_INI_SYSTEM, OnUpdateBool, cache_enable_cli, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.backend", "mmap", PHP_INI_SYSTEM, OnUpdateString, cache_backend, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.max_size", "67108864", PHP_INI_SYSTEM, OnUpdateLong, cache_max_size, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.max_entries", "349529", PHP_INI_SYSTEM, OnUpdateLong, cache_max_entries, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.max_age", "-1", PHP_INI_SYSTEM, OnUpdateLong, cache_max_age, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_ENTRY("handlebars.cache.save_path", "/tmp/php-handlebars-cache", PHP_INI_SYSTEM, OnUpdateString, cache_save_path, zend_handlebars_globals, handlebars_globals)
    STD_PHP_INI_BOOLEAN("handlebars.cache.stat", "1", PHP_INI_SYSTEM, OnUpdateBool, cache_stat, zend_handlebars_globals, handlebars_globals)
PHP_INI_END()
/* }}} */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(handlebars_cache_reset_args, ZEND_SEND_BY_VAL, 0, 0)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ proto void handlebars_cache_reset(void) */
PHP_FUNCTION(handlebars_cache_reset)
{
    if (HANDLEBARS_G(cache_enable) && HANDLEBARS_G(cache)) {
        handlebars_cache_reset(HANDLEBARS_G(cache));
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_RINIT_FUNCTION(handlebars)
{
#if defined(COMPILE_DL_HANDLEBARS) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(handlebars)
{
    int flags = CONST_CS | CONST_PERSISTENT;
    const char * version = handlebars_version_string();

    REGISTER_INI_ENTRIES();

    // Register constants
    if (zend_hash_str_exists(&module_registry, "psr", sizeof("psr") - 1)) {
        handlebars_has_psr = 1;
    }
    REGISTER_LONG_CONSTANT("Handlebars\\PSR", handlebars_has_psr, flags);

    REGISTER_STRING_CONSTANT("Handlebars\\VERSION", (char *) PHP_HANDLEBARS_VERSION, flags);
    REGISTER_STRING_CONSTANT("Handlebars\\LIBVERSION", (char *) version, flags);
    REGISTER_STRING_CONSTANT("Handlebars\\LIBVERSION2", (char *) HANDLEBARS_VERSION_STRING, flags);

    // Setup root contexts
    HANDLEBARS_G(root) = talloc_new(NULL);
    HANDLEBARS_G(context) = handlebars_context_ctor_ex(HANDLEBARS_G(root));

    // Setup cache
    if( !HANDLEBARS_G(cache_enable_cli) && 0 == strcmp(sapi_module.name, "cli") ) {
        HANDLEBARS_G(cache_enable) = false;
    }

    // Save jmp
    jmp_buf buf;

    if( handlebars_setjmp_ex(HANDLEBARS_G(context), &buf) ) {
        HANDLEBARS_G(cache_enable) = 0;
    }

    const char * backend = NULL;
    if (HANDLEBARS_G(cache_enable)) {
        backend = HANDLEBARS_G(cache_backend);
        if( strcmp(backend, "simple") == 0 ) {
            HANDLEBARS_G(cache) = handlebars_cache_simple_ctor(HANDLEBARS_G(context));
#ifdef HANDLEBARS_HAVE_LMDB
        } else if( strcmp(backend, "lmdb") == 0 ) {
            HANDLEBARS_G(cache) = handlebars_cache_lmdb_ctor(HANDLEBARS_G(context), HANDLEBARS_G(cache_save_path));
#endif
#ifdef HANDLEBARS_HAVE_PTHREAD
        } else if( strcmp(backend, "mmap") == 0 ) {
            HANDLEBARS_G(cache) = handlebars_cache_mmap_ctor(HANDLEBARS_G(context), HANDLEBARS_G(cache_max_size), HANDLEBARS_G(cache_max_entries));
#endif
        } else {
            backend = NULL;
        }
        // @TODO FIXME
        // if( strcmp(backend, "mmap") != 0 ) {
        //     HANDLEBARS_G(cache)->max_entries = (size_t) HANDLEBARS_G(cache_max_entries);
        //     HANDLEBARS_G(cache)->max_size = (size_t) HANDLEBARS_G(cache_max_size);
        // }
        // HANDLEBARS_G(cache)->max_age = (double) HANDLEBARS_G(cache_max_age);
    }

    if (backend) {
        REGISTER_STRING_CONSTANT("Handlebars\\CACHE_BACKEND", (char *) backend, flags);
    } else {
#ifdef REGISTER_NULL_CONSTANT
        REGISTER_NULL_CONSTANT("Handlebars\\CACHE_BACKEND", flags);
#else
        REGISTER_LONG_CONSTANT("Handlebars\\CACHE_BACKEND", 0, flags);
#endif
    }

    // Call other MINIT functions
    PHP_MINIT(handlebars_registry)(INIT_FUNC_ARGS_PASSTHRU); // must be before impl
    PHP_MINIT(handlebars_impl)(INIT_FUNC_ARGS_PASSTHRU); // must be before vm
    PHP_MINIT(handlebars_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_options)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_safe_string)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_utils)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_value)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_vm)(INIT_FUNC_ARGS_PASSTHRU);

#ifdef PHP_HANDLEBARS_ENABLE_AST
    PHP_MINIT(handlebars_compiler)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_opcode)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_parser)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_program)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_token)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(handlebars_tokenizer)(INIT_FUNC_ARGS_PASSTHRU);
#endif

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
    php_info_print_table_row(2, "PSR support", handlebars_has_psr ? "active" : "inactive");
    php_info_print_table_row(2, "libhandlebars Version", handlebars_version_string());
    php_info_print_table_row(2, "libhandlebars Version (compile-time)", HANDLEBARS_VERSION_STRING);
    php_info_print_table_row(2, "libhandlebars Handlebars Spec Version", handlebars_spec_version_string());
    php_info_print_table_row(2, "libhandlebars Mustache Spec Version", handlebars_mustache_spec_version_string());
    php_info_print_table_row(2, "xxhash version",  HANDLEBARS_XXHASH_VERSION);

    snprintf(buf, sizeof(buf), "%zu", talloc_total_size(HANDLEBARS_G(root)));
    php_info_print_table_row(2, "Local memory usage", buf);
    php_info_print_table_end();

    if( HANDLEBARS_G(cache) ) {
        struct handlebars_cache_stat stat = handlebars_cache_stat(HANDLEBARS_G(cache));

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif

        php_info_print_table_start();
        php_info_print_table_colspan_header(2, (char *) "Cache");

        php_info_print_table_row(2, "Backend", stat.name);

        snprintf(buf, sizeof(buf), "%zu", stat.total_size);
        php_info_print_table_row(2, "Block size", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.total_table_size);
        php_info_print_table_row(2, "Table size", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.total_entries);
        php_info_print_table_row(2, "Table entries", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.current_entries);
        php_info_print_table_row(2, "Table entries used", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.total_entries > 0 ? stat.total_entries - stat.current_entries : 0);
        php_info_print_table_row(2, "Table entries free", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.total_data_size);
        php_info_print_table_row(2, "Data segment size", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.current_data_size);
        php_info_print_table_row(2, "Data segment used", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.total_data_size > 0 ? stat.total_data_size - stat.current_data_size : 0);
        php_info_print_table_row(2, "Data segment free", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.hits);
        php_info_print_table_row(2, "Hits", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.misses);
        php_info_print_table_row(2, "Misses", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.refcount);
        php_info_print_table_row(2, "Refcount", buf);

        snprintf(buf, sizeof(buf), "%zu", stat.collisions);
        php_info_print_table_row(2, "Collisions", buf);

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

        php_info_print_table_end();
    }

    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
PHP_GINIT_FUNCTION(handlebars)
{
#if defined(COMPILE_DL_HANDLEBARS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(handlebars_globals, 0, sizeof(zend_handlebars_globals));
    handlebars_globals->pool_size = 0; // @TODO FIXME 128 * 1024;
    handlebars_globals->cache_enable = 1;
    handlebars_globals->cache_backend = "mmap";
    handlebars_globals->cache_save_path = "/tmp/php-handlebars-cache";
    handlebars_globals->cache_max_age = -1;
    handlebars_globals->cache_max_entries = 349529;
    handlebars_globals->cache_max_size = 52428800;
}
/* }}} */

/* {{{ handlebars_functions
 */
const zend_function_entry handlebars_functions[] = {
    PHP_FE(handlebars_cache_reset, handlebars_cache_reset_args)
    PHP_FE_END
};
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
    handlebars_functions,               /* Functions */
    PHP_MINIT(handlebars),              /* MINIT */
    PHP_MSHUTDOWN(handlebars),          /* MSHUTDOWN */
    PHP_RINIT(handlebars),              /* RINIT */
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
#if defined(ZTS)
    ZEND_TSRMLS_CACHE_DEFINE()
#endif
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
