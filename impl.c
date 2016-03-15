
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

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsImpl_ce_ptr;
zend_class_entry * HandlebarsBaseImpl_ce_ptr;
/* }}} Variables & Prototypes */

static zend_class_entry *lookup_class(const char *name TSRMLS_DC)
{
    zend_class_entry * ce = NULL;
    strsize_t len = strlen(name);
#ifdef ZEND_ENGINE_3
    zend_string * key = zend_string_alloc(len, 0);
    zend_str_tolower_copy(ZSTR_VAL(key), name, len);
    ce = zend_hash_find_ptr(CG(class_table), key);
    zend_string_free(key);
#else
    char * key = emalloc(len + 1);
    zend_str_tolower_copy(key, name, len);
    zend_class_entry ** pce;
    if( zend_hash_find(CG(class_table), key, len + 1, (void **) &pce) == SUCCESS ) {
        ce = *pce;
    }
    efree(key);
#endif
    if( NULL == ce ) {
        zend_error(E_ERROR, "Class %s not found", name);
    }
    return ce;
}

static inline void php_handlebars_impl_getter(INTERNAL_FUNCTION_PARAMETERS, const char * str, strsize_t len)
{
    zval * _this_zval;
    zval * val;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O",
            &_this_zval, HandlebarsBaseImpl_ce_ptr) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
#endif

    val = php5to7_zend_read_property2(Z_OBJCE_P(_this_zval), _this_zval, str, len, 1);
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

/* {{{ proto mixed Handlebars\BaseImpl::setHelpers(Handlebars\Registry $helpers) */
PHP_METHOD(HandlebarsBaseImpl, setHelpers)
{
    zval * _this_zval;
    zval * helpers;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "OO",
            &_this_zval, HandlebarsBaseImpl_ce_ptr, &helpers, HandlebarsRegistry_ce_ptr) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(helpers, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();
#endif

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("helpers"), helpers TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Handlebars\BaseImpl::setPartials(Handlebars\Registry $partials) */
PHP_METHOD(HandlebarsBaseImpl, setPartials)
{
    zval * _this_zval;
    zval * partials;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "OO",
            &_this_zval, HandlebarsBaseImpl_ce_ptr, &partials, HandlebarsRegistry_ce_ptr) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(partials, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();
#endif

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("partials"), partials TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Handlebars\BaseImpl::setDecorators(Handlebars\Registry $decorators) */
PHP_METHOD(HandlebarsBaseImpl, setDecorators)
{
    zval * _this_zval;
    zval * decorators;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "OO",
            &_this_zval, HandlebarsBaseImpl_ce_ptr, &decorators, HandlebarsRegistry_ce_ptr) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(decorators, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();
#endif

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("decorators"), decorators TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Handlebars\BaseImpl::setLogger(Psr\Log\LoggerInterface $logger) */
PHP_METHOD(HandlebarsBaseImpl, setLogger)
{
    zval * _this_zval;
    zval * logger;
    zend_class_entry * PsrLogLoggerInterface_ce_ptr = lookup_class("Psr\\Log\\LoggerInterface" TSRMLS_CC);

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO",
                                     &_this_zval, NULL, &logger, PsrLogLoggerInterface_ce_ptr) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
	ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(logger, PsrLogLoggerInterface_ce_ptr)
	ZEND_PARSE_PARAMETERS_END();
#endif

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("logger"), logger TSRMLS_CC);
}
/* }}} */

/* {{{ Handlebars\HandlebarsImpl methods */
static zend_function_entry HandlebarsImpl_methods[] = {
    PHP_ABSTRACT_ME(HandlebarsImpl, getHelpers, HandlebarsImpl_getHelpers_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, getPartials, HandlebarsImpl_getHelpers_args)
    PHP_ABSTRACT_ME(HandlebarsImpl, getDecorators, HandlebarsImpl_getHelpers_args)
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
    HandlebarsImpl_ce_ptr = zend_register_internal_interface(&ce TSRMLS_CC);

    zend_declare_class_constant_string(HandlebarsImpl_ce_ptr, ZEND_STRL("MODE_COMPILER"), "compiler" TSRMLS_CC);
    zend_declare_class_constant_string(HandlebarsImpl_ce_ptr, ZEND_STRL("MODE_VM"), "vm" TSRMLS_CC);
    zend_declare_class_constant_string(HandlebarsImpl_ce_ptr, ZEND_STRL("MODE_CVM"), "cvm" TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "Handlebars\\BaseImpl", HandlebarsBaseImpl_methods);
    HandlebarsBaseImpl_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_class_implements(HandlebarsBaseImpl_ce_ptr TSRMLS_CC, 1, HandlebarsImpl_ce_ptr);

    if( handlebars_has_psr ) {
        zend_class_entry *tmp = lookup_class("Psr\\Log\\LoggerAwareInterface" TSRMLS_CC);
        if( tmp ) {
            zend_class_implements(HandlebarsBaseImpl_ce_ptr TSRMLS_CC, 1, tmp);
        } else {
            return FAILURE;
        }
    }

    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("helpers"), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("partials"), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("decorators"), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(HandlebarsBaseImpl_ce_ptr, ZEND_STRL("logger"), ZEND_ACC_PROTECTED TSRMLS_CC);

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
