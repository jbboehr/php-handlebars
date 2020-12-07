
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"

//#ifdef HAVE_HANDLEBARS_PSR
//#include "ext/psr/psr_log.h"
//#endif

#include "php_handlebars.h"

#include "php7to8.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsImpl_ce_ptr;
PHP_HANDLEBARS_API zend_class_entry * HandlebarsBaseImpl_ce_ptr;
zend_string *HANDLEBARS_INTERNED_STR_LOGGER;
zend_string *HANDLEBARS_INTERNED_STR_DECORATORS;
zend_string *HANDLEBARS_INTERNED_STR_HELPERS;
zend_string *HANDLEBARS_INTERNED_STR_PARTIALS;
/* }}} Variables & Prototypes */

static zend_class_entry *lookup_class(const char *name)
{
    zend_class_entry * ce = NULL;
    size_t len = strlen(name);
    zend_string * key = zend_string_alloc(len, 0);
    zend_str_tolower_copy(ZSTR_VAL(key), name, len);
    ce = zend_hash_find_ptr(CG(class_table), key);
    zend_string_free(key);
    if( NULL == ce ) { // LCOV_EXCL_START
        zend_error(E_ERROR, "Class %s not found", name);
    } // LCOV_EXCL_STOP
    return ce;
}

static inline void php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAMETERS, zend_string *str)
{
    zval * _this_zval = getThis();
    PHP_HBS_ASSERT(_this_zval);
    zval * val = zend_read_property_ex(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), str, 1, NULL);
    RETURN_ZVAL(val, 1, 0);
}

PHP_METHOD(HandlebarsBaseImpl, getHelpers)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, HANDLEBARS_INTERNED_STR_HELPERS);
}

PHP_METHOD(HandlebarsBaseImpl, getPartials)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, HANDLEBARS_INTERNED_STR_PARTIALS);
}

PHP_METHOD(HandlebarsBaseImpl, getDecorators)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, HANDLEBARS_INTERNED_STR_DECORATORS);
}

PHP_METHOD(HandlebarsBaseImpl, getLogger)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, HANDLEBARS_INTERNED_STR_LOGGER);
}

/* {{{ proto mixed Handlebars\BaseImpl::setHelpers(Handlebars\Registry $helpers) */
PHP_METHOD(HandlebarsBaseImpl, setHelpers)
{
    zval * _this_zval = getThis();
    zval * helpers;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(helpers, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_ex(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), HANDLEBARS_INTERNED_STR_HELPERS, helpers);

    RETURN_ZVAL(_this_zval, 1, 0);
}
/* }}} */

/* {{{ proto mixed Handlebars\BaseImpl::setPartials(Handlebars\Registry $partials) */
PHP_METHOD(HandlebarsBaseImpl, setPartials)
{
    zval * _this_zval = getThis();
    zval * partials;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(partials, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_ex(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), HANDLEBARS_INTERNED_STR_PARTIALS, partials);

    RETURN_ZVAL(_this_zval, 1, 0);
}
/* }}} */

/* {{{ proto mixed Handlebars\BaseImpl::setDecorators(Handlebars\Registry $decorators) */
PHP_METHOD(HandlebarsBaseImpl, setDecorators)
{
    zval * _this_zval = getThis();
    zval * decorators;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(decorators, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_ex(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), HANDLEBARS_INTERNED_STR_DECORATORS, decorators);

    RETURN_ZVAL(_this_zval, 1, 0);
}
/* }}} */

/* {{{ proto mixed Handlebars\BaseImpl::setLogger(Psr\Log\LoggerInterface $logger) */
PHP_METHOD(HandlebarsBaseImpl, setLogger)
{
    zval * _this_zval = getThis();
    zval * logger;
    zend_class_entry * PsrLogLoggerInterface_ce_ptr = lookup_class("Psr\\Log\\LoggerInterface");

	ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(logger, PsrLogLoggerInterface_ce_ptr)
	ZEND_PARSE_PARAMETERS_END();

    zend_update_property_ex(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), HANDLEBARS_INTERNED_STR_LOGGER, logger);

    RETURN_ZVAL(_this_zval, 1, 0);
}
/* }}} */

/* {{{ Handlebars\HandlebarsImpl methods */
static zend_function_entry HandlebarsImpl_methods[] = {
    PHP_ABSTRACT_ME(HandlebarsImpl, getHelpers, arginfo_HandlebarsImpl_getHelpers)
    PHP_ABSTRACT_ME(HandlebarsImpl, getPartials, arginfo_HandlebarsImpl_getPartials)
    PHP_ABSTRACT_ME(HandlebarsImpl, getDecorators, arginfo_HandlebarsImpl_getDecorators)
    PHP_ABSTRACT_ME(HandlebarsImpl, getLogger, arginfo_HandlebarsImpl_getLogger)
    PHP_ABSTRACT_ME(HandlebarsImpl, setHelpers, arginfo_HandlebarsImpl_setHelpers)
    PHP_ABSTRACT_ME(HandlebarsImpl, setPartials, arginfo_HandlebarsImpl_setPartials)
    PHP_ABSTRACT_ME(HandlebarsImpl, setDecorators, arginfo_HandlebarsImpl_setDecorators)
    PHP_ABSTRACT_ME(HandlebarsImpl, setLogger, arginfo_HandlebarsImpl_setLogger)
    PHP_ABSTRACT_ME(HandlebarsImpl, render, arginfo_HandlebarsImpl_render)
    PHP_ABSTRACT_ME(HandlebarsImpl, renderFile, arginfo_HandlebarsImpl_renderFile)
    PHP_FE_END
};
/* }}} Handlebars\HandlebarsImpl methods */

/* {{{ Handlebars\HandlebarsBaseImpl methods */
static zend_function_entry HandlebarsBaseImpl_methods[] = {
    PHP_ME(HandlebarsBaseImpl, getHelpers, arginfo_HandlebarsImpl_getHelpers, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, getPartials, arginfo_HandlebarsImpl_getPartials, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, getDecorators, arginfo_HandlebarsImpl_getDecorators, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, getLogger, arginfo_HandlebarsImpl_getLogger, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setHelpers, arginfo_HandlebarsImpl_setHelpers, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setPartials, arginfo_HandlebarsImpl_setPartials, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setDecorators, arginfo_HandlebarsImpl_setDecorators, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setLogger, arginfo_HandlebarsImpl_setLogger, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} Handlebars\HandlebarsBaseImpl methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_impl)
{
    zend_class_entry ce;

    HANDLEBARS_INTERNED_STR_LOGGER = zend_new_interned_string(zend_string_init(ZEND_STRL("logger"), 1));
    HANDLEBARS_INTERNED_STR_DECORATORS = zend_new_interned_string(zend_string_init(ZEND_STRL("decorators"), 1));
    HANDLEBARS_INTERNED_STR_HELPERS = zend_new_interned_string(zend_string_init(ZEND_STRL("helpers"), 1));
    HANDLEBARS_INTERNED_STR_PARTIALS = zend_new_interned_string(zend_string_init(ZEND_STRL("partials"), 1));

    INIT_CLASS_ENTRY(ce, "Handlebars\\Impl", HandlebarsImpl_methods);
    HandlebarsImpl_ce_ptr = zend_register_internal_interface(&ce);

    if( handlebars_has_psr ) {
        zend_class_entry *tmp = lookup_class("Psr\\Log\\LoggerAwareInterface");
        if( tmp ) {
            zend_class_implements(HandlebarsImpl_ce_ptr, 1, tmp);
        } else { // LCOV_EXCL_START
            return FAILURE;
        } // LCOV_EXCL_STOP
    }

    INIT_CLASS_ENTRY(ce, "Handlebars\\BaseImpl", HandlebarsBaseImpl_methods);
    HandlebarsBaseImpl_ce_ptr = zend_register_internal_class(&ce);
    zend_class_implements(HandlebarsBaseImpl_ce_ptr, 1, HandlebarsImpl_ce_ptr);

#if PHP_VERSION_ID >= 70400
	zval default_val;
	ZVAL_UNDEF(&default_val);

    zend_class_entry *ilogger_ce = NULL;
    if( handlebars_has_psr ) {
        ilogger_ce = lookup_class("Psr\\Log\\LoggerInterface");
    }
#endif

// Current TravisCI's PHP master is really old, falling through to PHP 7.4 case should fix it...
#if PHP_VERSION_ID >= 80000 && defined(ZEND_TYPE_INIT_CE)

    if( ilogger_ce ) {
        zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_LOGGER, &default_val, ZEND_ACC_PROTECTED, NULL,
            (zend_type) ZEND_TYPE_INIT_CE(ilogger_ce, 1, 0));
    } else {
        // checking zend.c:991, it appears that we can't typehint a userland class here, sadly
        // zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_LOGGER, &default_val, ZEND_ACC_PROTECTED, NULL,
        //     (zend_type) ZEND_TYPE_INIT_CLASS(zend_string_init(ZEND_STRL("psr\\log\\loggerinterface"), 1), 1, 0));
        zend_declare_property_ex(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_LOGGER, &default_val, ZEND_ACC_PROTECTED, NULL);
    }

	zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_DECORATORS, &default_val, ZEND_ACC_PROTECTED, NULL,
		(zend_type) ZEND_TYPE_INIT_CE(HandlebarsRegistry_ce_ptr, 1, 0));
	zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_HELPERS, &default_val, ZEND_ACC_PROTECTED, NULL,
		(zend_type) ZEND_TYPE_INIT_CE(HandlebarsRegistry_ce_ptr, 1, 0));
	zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_PARTIALS, &default_val, ZEND_ACC_PROTECTED, NULL,
		(zend_type) ZEND_TYPE_INIT_CE(HandlebarsRegistry_ce_ptr, 1, 0));

#elif PHP_VERSION_ID >= 70400

    if( ilogger_ce ) {
        zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_LOGGER, &default_val, ZEND_ACC_PROTECTED, NULL,
            ZEND_TYPE_ENCODE_CE(ilogger_ce, 1));
    } else {
        // Checking zend.c:971, it appears that we can't typehint a userland class here, sadly
        //zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_LOGGER, &default_val, ZEND_ACC_PROTECTED, NULL,
        //    ZEND_TYPE_ENCODE_CLASS(zend_string_init(ZEND_STRL("psr\\log\\loggerinterface"), 1), 1));
        zend_declare_property_ex(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_LOGGER, &default_val, ZEND_ACC_PROTECTED, NULL);
    }

	zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_DECORATORS, &default_val, ZEND_ACC_PROTECTED, NULL,
		ZEND_TYPE_ENCODE_CE(HandlebarsRegistry_ce_ptr, 1));
	zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_HELPERS, &default_val, ZEND_ACC_PROTECTED, NULL,
		ZEND_TYPE_ENCODE_CE(HandlebarsRegistry_ce_ptr, 1));
	zend_declare_typed_property(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_PARTIALS, &default_val, ZEND_ACC_PROTECTED, NULL,
		ZEND_TYPE_ENCODE_CE(HandlebarsRegistry_ce_ptr, 1));

#else

	zval default_val;
	ZVAL_NULL(&default_val);
    zend_declare_property_ex(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_LOGGER, &default_val, ZEND_ACC_PROTECTED, NULL);
    zend_declare_property_ex(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_DECORATORS, &default_val, ZEND_ACC_PROTECTED, NULL);
    zend_declare_property_ex(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_HELPERS, &default_val, ZEND_ACC_PROTECTED, NULL);
    zend_declare_property_ex(HandlebarsBaseImpl_ce_ptr, HANDLEBARS_INTERNED_STR_PARTIALS, &default_val, ZEND_ACC_PROTECTED, NULL);

#endif

    return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
