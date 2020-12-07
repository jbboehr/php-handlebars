
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php_handlebars.h"

#include "php7to8.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsProgram_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsProgram_construct_args, ZEND_SEND_BY_VAL, 0, 2)
	ZEND_ARG_ARRAY_INFO(0, opcodes, 0)
	ZEND_ARG_ARRAY_INFO(0, children, 0)
    ZEND_ARG_TYPE_INFO(0, blockParams, IS_LONG, 0)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ proto Handlebars\Program::__construct(array opcodes, array children, integer blockParams) */
PHP_METHOD(HandlebarsProgram, __construct)
{
    zval * _this_zval = getThis();
    zval * opcodes;
    zval * children;
    zend_long blockParams;

    ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ARRAY(opcodes)
		Z_PARAM_ARRAY(children)
		Z_PARAM_LONG(blockParams);
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), ZEND_STRL("opcodes"), opcodes);
    zend_update_property(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), ZEND_STRL("children"), children);
    zend_update_property_long(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), ZEND_STRL("blockParams"), blockParams);
}
/* }}} Handlebars\Program::__construct */

/* {{{ Handlebars\Program methods */
static zend_function_entry HandlebarsProgram_methods[] = {
    PHP_ME(HandlebarsProgram, __construct, HandlebarsProgram_construct_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} Handlebars\Program methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_program)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Program", HandlebarsProgram_methods);
    HandlebarsProgram_ce_ptr = zend_register_internal_class(&ce);

    zend_declare_property_null(HandlebarsProgram_ce_ptr, ZEND_STRL("opcodes"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(HandlebarsProgram_ce_ptr, ZEND_STRL("children"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(HandlebarsProgram_ce_ptr, ZEND_STRL("decorators"), ZEND_ACC_PUBLIC);
    zend_declare_property_bool(HandlebarsProgram_ce_ptr, ZEND_STRL("isSimple"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_bool(HandlebarsProgram_ce_ptr, ZEND_STRL("useDepths"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_bool(HandlebarsProgram_ce_ptr, ZEND_STRL("usePartial"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_bool(HandlebarsProgram_ce_ptr, ZEND_STRL("useDecorators"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_long(HandlebarsProgram_ce_ptr, ZEND_STRL("blockParams"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_bool(HandlebarsProgram_ce_ptr, ZEND_STRL("stringParams"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_bool(HandlebarsProgram_ce_ptr, ZEND_STRL("trackIds"), 0, ZEND_ACC_PUBLIC);

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
