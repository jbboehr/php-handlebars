
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsOpcode_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsOpcode_construct_args, ZEND_SEND_BY_VAL, 0, 2)
    ZEND_ARG_INFO(0, opcode)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ proto Handlebars\Opcode::__construct(string opcode, array args) */
PHP_METHOD(HandlebarsOpcode, __construct)
{
    zval * _this_zval = getThis();
    char * opcode_str;
    size_t opcode_len;
    zval * args;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(opcode_str, opcode_len)
		Z_PARAM_ARRAY(args)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_stringl(Z_OBJCE_P(_this_zval), _this_zval, "opcode", sizeof("opcode")-1, opcode_str, opcode_len);
    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, "args", sizeof("args")-1, args);
}
/* }}} Handlebars\Opcode::__construct */

/* {{{ HandlebarsOpcode methods */
static zend_function_entry HandlebarsOpcode_methods[] = {
    PHP_ME(HandlebarsOpcode, __construct, HandlebarsOpcode_construct_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} HandlebarsOpcode methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_opcode)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Opcode", HandlebarsOpcode_methods);
    HandlebarsOpcode_ce_ptr = zend_register_internal_class(&ce);

    zend_declare_property_null(HandlebarsOpcode_ce_ptr, "opcode", sizeof("opcode")-1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(HandlebarsOpcode_ce_ptr, "args", sizeof("args")-1, ZEND_ACC_PUBLIC);

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
