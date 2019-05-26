
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsSafeString_ce_ptr;
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
    zval * _this_zval = getThis();
    zend_string * value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STR(value)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_str(Z_OBJCE_P(_this_zval), _this_zval, "value", sizeof("value")-1, value);
}
/* }}} Handlebars\SafeString::__construct */

/* {{{ proto string Handlebars\SafeString::__toString() */
PHP_METHOD(HandlebarsSafeString, __toString)
{
    zval * _this_zval = getThis();
    zval rv;
    zval * value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("value"), 1, &rv);
    RETURN_ZVAL(value, 1, 0);
}
/* }}} HandlebarsSafeString::__toString */

/* {{{ HandlebarsSafeString methods */
static zend_function_entry HandlebarsSafeString_methods[] = {
    PHP_ME(HandlebarsSafeString, __construct, HandlebarsSafeString_construct_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsSafeString, __toString, HandlebarsSafeString_toString_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} HandlebarsSafeString methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_safe_string)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\SafeString", HandlebarsSafeString_methods);
    HandlebarsSafeString_ce_ptr = zend_register_internal_class(&ce);
    zend_declare_property_null(HandlebarsSafeString_ce_ptr, "value", sizeof("value")-1, ZEND_ACC_PROTECTED);

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
