
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsSafeString_ce_ptr;
static zend_string *INTERNED_VALUE;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
PHP_HANDLEBARS_BEGIN_ARG_INFO(HandlebarsSafeString, __construct, 1)
    ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsSafeString, __toString, 0, IS_STRING, 0)
PHP_HANDLEBARS_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ proto Handlebars\SafeString::__construct(string value) */
PHP_METHOD(HandlebarsSafeString, __construct)
{
    zval * _this_zval = getThis();
    zval tmp;
    zend_string * value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STR(value)
    ZEND_PARSE_PARAMETERS_END();

	ZVAL_STR(&tmp, value);
    zend_update_property_ex(Z_OBJCE_P(_this_zval), _this_zval, INTERNED_VALUE, &tmp);
}
/* }}} Handlebars\SafeString::__construct */

/* {{{ proto string Handlebars\SafeString::__toString() */
PHP_METHOD(HandlebarsSafeString, __toString)
{
    zval * _this_zval = getThis();
    zval rv;
    zval * value = zend_read_property_ex(Z_OBJCE_P(_this_zval), _this_zval, INTERNED_VALUE, 1, &rv);
    RETURN_ZVAL(value, 1, 0);
}
/* }}} HandlebarsSafeString::__toString */

/* {{{ HandlebarsSafeString methods */
static zend_function_entry HandlebarsSafeString_methods[] = {
    PHP_ME(HandlebarsSafeString, __construct, arginfo_HandlebarsSafeString___construct, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsSafeString, __toString, arginfo_HandlebarsSafeString___toString, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} HandlebarsSafeString methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_safe_string)
{
    zend_class_entry ce;
	zval default_val;

    INTERNED_VALUE = zend_new_interned_string(zend_string_init(ZEND_STRL("value"), 1));

    INIT_CLASS_ENTRY(ce, "Handlebars\\SafeString", HandlebarsSafeString_methods);
    HandlebarsSafeString_ce_ptr = zend_register_internal_class(&ce);

#if PHP_VERSION_ID >= 80000

	ZVAL_UNDEF(&default_val);
	zend_declare_typed_property(HandlebarsSafeString_ce_ptr, INTERNED_VALUE, &default_val, ZEND_ACC_PROTECTED, NULL,
		(zend_type) ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0));

#elif PHP_VERSION_ID >= 70400

	ZVAL_UNDEF(&default_val);
	zend_declare_typed_property(HandlebarsSafeString_ce_ptr, INTERNED_VALUE, &default_val, ZEND_ACC_PROTECTED, NULL,
		ZEND_TYPE_ENCODE(IS_STRING, 0));

#else

	ZVAL_NULL(&default_val);
    zend_declare_property_ex(HandlebarsSafeString_ce_ptr, INTERNED_VALUE, &default_val, ZEND_ACC_PROTECTED, NULL);

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
