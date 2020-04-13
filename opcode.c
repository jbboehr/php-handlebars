
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsOpcode_ce_ptr;
static zend_string *INTERNED_OPCODE;
static zend_string *INTERNED_ARGS;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsOpcode_construct_args, ZEND_SEND_BY_VAL, 0, 2)
    ZEND_ARG_TYPE_INFO(0, opcode, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, args, 0)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ proto Handlebars\Opcode::__construct(string opcode, array args) */
PHP_METHOD(HandlebarsOpcode, __construct)
{
    zval * _this_zval = getThis();
    zend_string * opcode;
    zval * args;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(opcode)
		Z_PARAM_ARRAY(args)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_str(Z_OBJCE_P(_this_zval), _this_zval, "opcode", sizeof("opcode")-1, opcode);
    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, "args", sizeof("args")-1, args);
}
/* }}} Handlebars\Opcode::__construct */

/* {{{ HandlebarsOpcode methods */
static zend_function_entry HandlebarsOpcode_methods[] = {
    PHP_ME(HandlebarsOpcode, __construct, HandlebarsOpcode_construct_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} HandlebarsOpcode methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_opcode)
{
    zend_class_entry ce;
	zval default_val;

    INTERNED_OPCODE = zend_new_interned_string(zend_string_init(ZEND_STRL("opcode"), 1));
    INTERNED_ARGS = zend_new_interned_string(zend_string_init(ZEND_STRL("args"), 1));

    INIT_CLASS_ENTRY(ce, "Handlebars\\Opcode", HandlebarsOpcode_methods);
    HandlebarsOpcode_ce_ptr = zend_register_internal_class(&ce);

#if PHP_VERSION_ID >= 80000

	ZVAL_UNDEF(&default_val);
	zend_declare_typed_property(HandlebarsOpcode_ce_ptr, INTERNED_OPCODE, &default_val, ZEND_ACC_PUBLIC, NULL,
            (zend_type) ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0));
	zend_declare_typed_property(HandlebarsOpcode_ce_ptr, INTERNED_ARGS, &default_val, ZEND_ACC_PUBLIC, NULL,
            (zend_type) ZEND_TYPE_INIT_CODE(IS_ARRAY, 0, 0));

#elif PHP_VERSION_ID >= 70400

	ZVAL_UNDEF(&default_val);
	zend_declare_typed_property(HandlebarsOpcode_ce_ptr, INTERNED_OPCODE, &default_val, ZEND_ACC_PUBLIC, NULL,
            ZEND_TYPE_ENCODE(IS_STRING, 0));
	zend_declare_typed_property(HandlebarsOpcode_ce_ptr, INTERNED_ARGS, &default_val, ZEND_ACC_PUBLIC, NULL,
            ZEND_TYPE_ENCODE(IS_ARRAY, 0));

#else

	ZVAL_NULL(&default_val);
    zend_declare_property_ex(HandlebarsOpcode_ce_ptr, INTERNED_OPCODE, &default_val, ZEND_ACC_PUBLIC);
    zend_declare_property_ex(HandlebarsOpcode_ce_ptr, INTERNED_ARGS, &default_val, ZEND_ACC_PUBLIC);

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
