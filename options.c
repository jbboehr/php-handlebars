
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"
#include "ext/spl/spl_array.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsOptions_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsOptions_construct_args, ZEND_SEND_BY_VAL, 0, 2)
    ZEND_ARG_ARRAY_INFO(0, props, 1)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

PHPAPI void php_handlebars_options_ctor(struct handlebars_options * options, zval * z_options TSRMLS_DC)
{
    zval z_const;
    zval z_ret;

    object_init_ex(z_options, HandlebarsOptions_ce_ptr);

    do {
#if PHP_MAJOR_VERSION < 7
		ZVAL_STRING(&z_const, "__construct", 0);
	    call_user_function(&HandlebarsOptions_ce_ptr->function_table, &z_options, &z_const, &z_ret, 0, NULL TSRMLS_CC);
#else
        ZVAL_STRING(&z_const, "__construct");
        call_user_function(&HandlebarsOptions_ce_ptr->function_table, z_options, &z_const, &z_ret, 0, NULL TSRMLS_CC);
#endif
    } while(0);
}

/* {{{ proto Handlebars\Options::__construct([array $props]) */
PHP_METHOD(HandlebarsOptions, __construct)
{
    zval * _this_zval;
    zval * props;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|z",
            &_this_zval, HandlebarsOptions_ce_ptr, &props) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_ARRAY(props)
    ZEND_PARSE_PARAMETERS_END();
#endif

    if( Z_TYPE_P(props) == IS_ARRAY ) {
        HashTable * data_hash = NULL;
        HashPosition data_pointer = NULL;
        zval ** data_entry = NULL;
        char * key;
        int key_len;
        long index;
        long idx = 0;

        data_hash = Z_ARRVAL_P(props);

        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
            if (zend_hash_get_current_key_ex(data_hash, &key, &key_len, &index, 0, &data_pointer) == HASH_KEY_IS_STRING) {
                zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, key, key_len - 1, *data_entry TSRMLS_CC);
            }
            zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
    }
}
/* }}} Handlebars\Options::__construct */

/* {{{ Handlebars\Options methods */
static zend_function_entry HandlebarsOptions_methods[] = {
    PHP_ME(HandlebarsOptions, __construct, HandlebarsOptions_construct_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} Handlebars\Options methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_options)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Options", HandlebarsOptions_methods);
    HandlebarsOptions_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("name"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hash"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("fn"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("inverse"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("scope"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("data"), ZEND_ACC_PUBLIC TSRMLS_CC);

    // Used by handlebars.php
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("ids"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hashIds"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hashTypes"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hashContexts"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("types"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("contexts"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("args"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("partial"), ZEND_ACC_PUBLIC TSRMLS_CC);

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
