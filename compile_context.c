
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsCompileContext_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsCompileContext_construct_args, ZEND_SEND_BY_VAL, 0, 2)
	ZEND_ARG_ARRAY_INFO(0, opcodes, 0)
	ZEND_ARG_ARRAY_INFO(0, children, 0)
    ZEND_ARG_INFO(0, blockParams)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ proto Handlebars\CompileContext::__construct(array opcodes, array children, integer blockParams) */
PHP_METHOD(HandlebarsCompileContext, __construct)
{
    zval * _this_zval;
    zval * opcodes;
    zval * children;
    zend_long blockParams;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oaal",
            &_this_zval, HandlebarsCompileContext_ce_ptr, &opcodes, &children, &blockParams) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ARRAY(opcodes)
		Z_PARAM_ARRAY(children)
		Z_PARAM_LONG(blockParams);
    ZEND_PARSE_PARAMETERS_END();
#endif

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("opcodes"), opcodes TSRMLS_CC);
    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("children"), children TSRMLS_CC);
    zend_update_property_long(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("blockParams"), blockParams TSRMLS_CC);
}
/* }}} Handlebars\CompileContext::__construct */

/* {{{ HandlebarsCompileContext methods */
static zend_function_entry HandlebarsCompileContext_methods[] = {
    PHP_ME(HandlebarsCompileContext, __construct, HandlebarsCompileContext_construct_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} HandlebarsToken methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_compile_context)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\CompileContext", HandlebarsCompileContext_methods);
    HandlebarsCompileContext_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(HandlebarsCompileContext_ce_ptr, ZEND_STRL("opcodes"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsCompileContext_ce_ptr, ZEND_STRL("children"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_bool(HandlebarsCompileContext_ce_ptr, ZEND_STRL("isSimple"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_bool(HandlebarsCompileContext_ce_ptr, ZEND_STRL("useDepths"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_bool(HandlebarsCompileContext_ce_ptr, ZEND_STRL("usePartial"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_bool(HandlebarsCompileContext_ce_ptr, ZEND_STRL("useDecorators"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_long(HandlebarsCompileContext_ce_ptr, ZEND_STRL("blockParams"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);

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
