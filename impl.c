
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

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsImpl_ce_ptr;
PHP_HANDLEBARS_API zend_class_entry * HandlebarsBaseImpl_ce_ptr;
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

static inline void php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAMETERS, const char * str, size_t len)
{
    zval * _this_zval = getThis();
    zval * val = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, str, len, 1, NULL);
    RETURN_ZVAL(val, 1, 0);
}

PHP_METHOD(HandlebarsBaseImpl, getHelpers)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRL("helpers"));
}

PHP_METHOD(HandlebarsBaseImpl, getPartials)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRL("partials"));
}

PHP_METHOD(HandlebarsBaseImpl, getDecorators)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRL("decorators"));
}

PHP_METHOD(HandlebarsBaseImpl, getLogger)
{
    php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_STRL("logger"));
}

/* {{{ proto mixed Handlebars\BaseImpl::setHelpers(Handlebars\Registry $helpers) */
PHP_METHOD(HandlebarsBaseImpl, setHelpers)
{
    zval * _this_zval = getThis();
    zval * helpers;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(helpers, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("helpers"), helpers);
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

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("partials"), partials);
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

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("decorators"), decorators);
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

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("logger"), logger);
}
/* }}} */

/* {{{ Handlebars\HandlebarsImpl methods */
static zend_function_entry HandlebarsImpl_methods[] = {
    PHP_ABSTRACT_ME(HandlebarsImpl, getHelpers, HandlebarsImpl_getHelpers_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, getPartials, HandlebarsImpl_getHelpers_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, getDecorators, HandlebarsImpl_getHelpers_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, getLogger, HandlebarsImpl_getHelpers_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, setHelpers, HandlebarsImpl_setHelpers_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, setPartials, HandlebarsImpl_setPartials_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, setDecorators, HandlebarsImpl_setDecorators_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, setLogger, HandlebarsImpl_setLogger_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, render, HandlebarsImpl_render_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, renderFile, HandlebarsImpl_renderFile_args)
    PHP_FE_END
};
/* }}} Handlebars\HandlebarsImpl methods */

/* {{{ Handlebars\HandlebarsBaseImpl methods */
static zend_function_entry HandlebarsBaseImpl_methods[] = {
    PHP_ME(HandlebarsBaseImpl, getHelpers, HandlebarsImpl_getHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, getPartials, HandlebarsImpl_getHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, getDecorators, HandlebarsImpl_getHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, getLogger, HandlebarsImpl_getHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setHelpers, HandlebarsImpl_setHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setPartials, HandlebarsImpl_setPartials_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setDecorators, HandlebarsImpl_setDecorators_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsBaseImpl, setLogger, HandlebarsImpl_setLogger_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} Handlebars\HandlebarsBaseImpl methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_impl)
{
    zend_class_entry ce;

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

    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("helpers"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("partials"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("decorators"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("logger"), ZEND_ACC_PROTECTED);

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
