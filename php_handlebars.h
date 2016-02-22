
#ifndef PHP_HANDLEBARS_H
#define PHP_HANDLEBARS_H

#include "main/php.h"

#define PHP_HANDLEBARS_NAME "handlebars"
#define PHP_HANDLEBARS_VERSION "0.6.1"
#define PHP_HANDLEBARS_RELEASE "2015-10-31"
#define PHP_HANDLEBARS_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"
#define PHP_HANDLEBARS_SPEC "4.0.4"

struct handlebars_cache;
struct handlebars_context;
struct handlebars_options;
struct handlebars_token;

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

extern zend_class_entry * HandlebarsCompileContext_ce_ptr;
extern zend_class_entry * HandlebarsCompiler_ce_ptr;
extern zend_class_entry * HandlebarsException_ce_ptr;
extern zend_class_entry * HandlebarsParseException_ce_ptr;
extern zend_class_entry * HandlebarsCompileException_ce_ptr;
extern zend_class_entry * HandlebarsInvalidArgumentException_ce_ptr;
extern zend_class_entry * HandlebarsOpcode_ce_ptr;
extern zend_class_entry * HandlebarsOptions_ce_ptr;
extern zend_class_entry * HandlebarsParser_ce_ptr;
extern zend_class_entry * HandlebarsRegistry_ce_ptr;
extern zend_class_entry * HandlebarsDefaultRegistry_ce_ptr;
extern zend_class_entry * HandlebarsRuntimeException_ce_ptr;
extern zend_class_entry * HandlebarsSafeString_ce_ptr;
extern zend_class_entry * HandlebarsToken_ce_ptr;
extern zend_class_entry * HandlebarsTokenizer_ce_ptr;
extern zend_class_entry * HandlebarsUtils_ce_ptr;
extern zend_class_entry * HandlebarsVM_ce_ptr;

struct php_handlebars_cache_entry {
    struct handlebars_context * ctx;
    struct handlebars_compiler * compiler;
};

ZEND_BEGIN_MODULE_GLOBALS(handlebars)
    zend_long  pool_size;

    void * root;
    struct handlebars_context * context;
    struct handlebars_cache * cache;
ZEND_END_MODULE_GLOBALS(handlebars)

ZEND_EXTERN_MODULE_GLOBALS(handlebars);

zend_bool inline php_handlebars_is_callable(zval * var TSRMLS_DC);
zend_bool inline php_handlebars_is_int_array(zval * arr TSRMLS_DC);

PHPAPI void php_handlebars_options_ctor(struct handlebars_options * options, zval * z_options TSRMLS_DC);
PHPAPI void php_handlebars_token_ctor(struct handlebars_token * token, zval * z_token TSRMLS_DC);

PHPAPI struct handlebars_value * handlebars_value_from_zval(struct handlebars_context * context, zval * val TSRMLS_DC);
PHPAPI zval * handlebars_value_to_zval(struct handlebars_value * value, zval * val TSRMLS_DC);

#define php_handlebars_throw(ce, ctx) zend_throw_exception(ce, handlebars_error_message(HBSCTX(ctx)), HBSCTX(ctx)->num TSRMLS_CC)
#define php_handlebars_try(ce, ctx, buf) \
    do { \
        if( handlebars_setjmp_ex(ctx, (buf)) ) { \
            php_handlebars_throw(ce, ctx); \
            goto done; \
        } \
    } while(0)
#define php_handlebars_try_rethrow(ce, ctx1, ctx2, buf) \
    do { \
        if( handlebars_setjmp_ex(ctx2, (buf)) ) { \
            php_handlebars_throw(ce, ctx1); \
            goto done; \
        } \
    } while(0)

#endif	/* PHP_HANDLEBARS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
