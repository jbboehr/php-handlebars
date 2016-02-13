
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php5to7.h"
#include "php_handlebars.h"
#include "handlebars_token.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsToken_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsToken_construct_args, ZEND_SEND_BY_VAL, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

PHPAPI void php_handlebars_token_ctor(struct handlebars_token * token, zval * z_token TSRMLS_DC)
{
    zval z_const;
    zval z_ret;
    _DECLARE_ZVAL(name);
    _DECLARE_ZVAL(text);

    _ALLOC_INIT_ZVAL(name);
            PHP5TO7_ZVAL_STRING(name, (char *) handlebars_token_readable_type(token->token));

    _ALLOC_INIT_ZVAL(text);
    if( token->text ) {
        PHP5TO7_ZVAL_STRING(text, token->text);
    }

    object_init_ex(z_token, HandlebarsToken_ce_ptr);

    do {
#if PHP_MAJOR_VERSION < 7
        zval **z_const_args = emalloc(2 * sizeof(zval *));

		ZVAL_STRING(&z_const, "__construct", 0);
		z_const_args[0] = name;
		z_const_args[1] = text;

	    call_user_function(&HandlebarsToken_ce_ptr->function_table, &z_token, &z_const, &z_ret, 2, z_const_args TSRMLS_CC);

		efree(z_const_args);
#else
        zval z_const_args[2];

        ZVAL_STRING(&z_const, "__construct");
        z_const_args[0] = *name;
        z_const_args[1] = *text;

        call_user_function(&HandlebarsToken_ce_ptr->function_table, z_token, &z_const, &z_ret, 2, z_const_args TSRMLS_CC);

        zval_dtor(&z_const);
#endif
    } while(0);

    php5to7_zval_ptr_dtor(name);
    php5to7_zval_ptr_dtor(text);
}

/* {{{ proto Handlebars\Token::__construct(string name, string text) */
PHP_METHOD(HandlebarsToken, __construct)
{
    zval * _this_zval;
    char * name_str;
    strsize_t name_len;
    char * text_str;
    strsize_t text_len;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oss",
            &_this_zval, HandlebarsToken_ce_ptr, &name_str, &name_len, &text_str, &text_len) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STRING(name_str, name_len)
    Z_PARAM_STRING(text_str, text_len)
    ZEND_PARSE_PARAMETERS_END();
#endif

    zend_update_property_stringl(Z_OBJCE_P(_this_zval), _this_zval, "name", sizeof("name")-1, name_str, name_len TSRMLS_CC);
    zend_update_property_stringl(Z_OBJCE_P(_this_zval), _this_zval, "text", sizeof("text")-1, text_str, text_len TSRMLS_CC);
}
/* }}} Handlebars\Token::__construct */

/* {{{ HandlebarsToken methods */
static zend_function_entry HandlebarsToken_methods[] = {
    PHP_ME(HandlebarsToken, __construct, HandlebarsToken_construct_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} HandlebarsToken methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_token)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Token", HandlebarsToken_methods);
    HandlebarsToken_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(HandlebarsToken_ce_ptr, "name", sizeof("name")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsToken_ce_ptr, "text", sizeof("text")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

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
