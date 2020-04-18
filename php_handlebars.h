
#ifndef PHP_HANDLEBARS_H
#define PHP_HANDLEBARS_H

#include "main/php.h"

#define PHP_HANDLEBARS_NAME "handlebars"
#define PHP_HANDLEBARS_VERSION "0.9.1"
#define PHP_HANDLEBARS_RELEASE "2020-04-18"
#define PHP_HANDLEBARS_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"
#define PHP_HANDLEBARS_SPEC "4.0.5"

#if defined(PHP_WIN32) && defined(HANDLEBARS_EXPORTS)
#define PHP_HANDLEBARS_API __declspec(dllexport)
#else
#define PHP_HANDLEBARS_API PHPAPI
#endif

struct handlebars_cache;
struct handlebars_compiler;
struct handlebars_context;
struct handlebars_options;
struct handlebars_token;
struct handlebars_vm;

extern zend_module_entry handlebars_module_entry;
#define phpext_handlebars_ptr &handlebars_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define HANDLEBARS_G(v) TSRMG(handlebars_globals_id, zend_handlebars_globals *, v)
#else
#define HANDLEBARS_G(v) (handlebars_globals.v)
#endif

PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsImpl_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsBaseImpl_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsCompiler_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsException_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsParseException_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsCompileException_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsInvalidArgumentException_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsInvalidBinaryStringException_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsOpcode_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsOptions_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsParser_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsProgram_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsRegistry_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsDefaultRegistry_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsRuntimeException_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsSafeString_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsToken_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsTokenizer_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsUtils_ce_ptr;
PHP_HANDLEBARS_API extern zend_class_entry * HandlebarsVM_ce_ptr;

ZEND_BEGIN_MODULE_GLOBALS(handlebars)
    zend_long pool_size;
    zend_bool cache_enable;
    zend_bool cache_enable_cli;
    const char * cache_backend;
    const char * cache_save_path;
    zend_long cache_max_size;
    zend_long cache_max_entries;
    zend_long cache_max_age;
    zend_bool cache_stat;

    void * root;
    struct handlebars_context * context;
    struct handlebars_cache * cache;
ZEND_END_MODULE_GLOBALS(handlebars)

ZEND_EXTERN_MODULE_GLOBALS(handlebars);

PHP_HANDLEBARS_API extern zend_bool handlebars_has_psr;

PHP_HANDLEBARS_API zend_bool php_handlebars_is_callable(zval * var);
PHP_HANDLEBARS_API zend_bool php_handlebars_is_int_array(zval * arr);

PHP_HANDLEBARS_API void php_handlebars_options_ctor(struct handlebars_options * options, zval * z_options);
PHP_HANDLEBARS_API void php_handlebars_token_ctor(struct handlebars_token * token, zval * z_token);
PHP_HANDLEBARS_API void php_handlebars_process_options_zval(struct handlebars_compiler * compiler, struct handlebars_vm * vm, zval * options);

PHP_HANDLEBARS_API struct handlebars_value * handlebars_value_from_zval(struct handlebars_context * context, zval * val);
PHP_HANDLEBARS_API zval * handlebars_value_to_zval(struct handlebars_value * value, zval * val);

#define php_handlebars_throw(ce, ctx) \
    do { \
        int num = handlebars_error_num(HBSCTX(ctx)); \
        if( num != HANDELBARS_EXTERNAL_ERROR ) { \
            zend_throw_exception(ce, handlebars_error_message(HBSCTX(ctx)), num); \
        } \
        goto done; \
    } while(0)
#define php_handlebars_try(ce, ctx, buf) \
    do { \
        if( handlebars_setjmp_ex(ctx, (buf)) ) { \
            php_handlebars_throw(ce, ctx); \
        } \
    } while(0)
#define php_handlebars_try_rethrow(ce, ctx1, ctx2, buf) \
    do { \
        if( handlebars_setjmp_ex(ctx2, (buf)) ) { \
            php_handlebars_throw(ce, ctx1); \
        } \
    } while(0)

#define PHP_HANDLEBARS_BEGIN_ARG_INFO(c, f, n) ZEND_BEGIN_ARG_INFO_EX(arginfo_ ## c ## _ ## f, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, n)
#define PHP_HANDLEBARS_END_ARG_INFO ZEND_END_ARG_INFO

#ifdef ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX
#define PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(c, f, n, cn, an) ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_ ## c ## _ ## f, ZEND_RETURN_VALUE, n, cn, an)
#define PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(c, f, n, t, an) ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ ## c ## _ ## f, ZEND_RETURN_VALUE, n, t, an)
#else
#define PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(c, f, n, cn, an) ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ ## c ## _ ## f, ZEND_RETURN_VALUE, n, IS_OBJECT, #cn, an)
#define PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(c, f, n, t, an) ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ ## c ## _ ## f, ZEND_RETURN_VALUE, n, t, NULL, an)
#endif

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, getLogger, 0, Psr\\Logger\\LoggerInterface, 1)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, getDecorators, 0, Handlebars\\Registry, 1)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, getHelpers, 0, Handlebars\\Registry, 1)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, getPartials, 0, Handlebars\\Registry, 1)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, setHelpers, 1, Handlebars\\Impl, 0)
    ZEND_ARG_OBJ_INFO(0, helpers, Handlebars\\Registry, 0)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, setPartials, 1, Handlebars\\Impl, 0)
    ZEND_ARG_OBJ_INFO(0, partials, Handlebars\\Registry, 0)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, setDecorators, 1, Handlebars\\Impl, 0)
    ZEND_ARG_OBJ_INFO(0, decorators, Handlebars\\Registry, 0)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_OBJ_INFO(HandlebarsImpl, setLogger, 1, Handlebars\\Impl, 0)
    ZEND_ARG_OBJ_INFO(0, logger, Psr\\Log\\LoggerInterface, 0)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsImpl, render, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
    ZEND_ARG_INFO(0, context)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsImpl, renderFile, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
    ZEND_ARG_INFO(0, context)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
PHP_HANDLEBARS_END_ARG_INFO()

// backwards compatibility
#define HandlebarsImpl_getHelpers_args arginfo_HandlebarsImpl_getHelpers
#define HandlebarsImpl_setHelpers_args arginfo_HandlebarsImpl_setHelpers
#define HandlebarsImpl_setPartials_args arginfo_HandlebarsImpl_setPartials
#define HandlebarsImpl_setDecorators_args arginfo_HandlebarsImpl_setDecorators
#define HandlebarsImpl_setLogger_args arginfo_HandlebarsImpl_setLogger
#define HandlebarsImpl_render_args arginfo_HandlebarsImpl_render
#define HandlebarsImpl_renderFile_args arginfo_HandlebarsImpl_renderFile

#endif	/* PHP_HANDLEBARS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
