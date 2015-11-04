
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsSafeString_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsSafeString_construct_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsSafeString_toString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ proto Handlebars\SafeString::__construct(string value) */
PHP_METHOD(HandlebarsSafeString, __construct)
{
    zval * _this_zval;
    char * value;
    strsize_t value_len;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os",
            &_this_zval, HandlebarsSafeString_ce_ptr, &value, &value_len) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STRING(value, value_len)
    ZEND_PARSE_PARAMETERS_END();
#endif

    zend_update_property_stringl(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, value, value_len TSRMLS_CC);
}
/* }}} Handlebars\SafeString::__construct */

/* {{{ proto string Handlebars\SafeString::__toString() */
PHP_METHOD(HandlebarsSafeString, __toString)
{
    zval * _this_zval;
    zval * value;
    zval rv;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
            &_this_zval, HandlebarsSafeString_ce_ptr) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
#endif

#if PHP_MAJOR_VERSION < 7
    value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, 1 TSRMLS_CC);
#else
    value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, 1, &rv TSRMLS_CC);
#endif
    RETURN_ZVAL(value, 1, 0);
}
/* }}} HandlebarsSafeString::__toString */

/* {{{ HandlebarsSafeString methods */
static zend_function_entry HandlebarsSafeString_methods[] = {
    PHP_ME(HandlebarsSafeString, __construct, HandlebarsSafeString_construct_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsSafeString, __toString, HandlebarsSafeString_toString_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} HandlebarsSafeString methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_safe_string)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\SafeString", HandlebarsSafeString_methods);
    HandlebarsSafeString_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(HandlebarsSafeString_ce_ptr, "value", sizeof("value")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

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
