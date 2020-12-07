
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "php_handlebars.h"

#define HANDLEBARS_AST_PRIVATE
#define HANDLEBARS_AST_LIST_PRIVATE

#include "handlebars_ast.h"
#include "handlebars_ast_list.h"
#include "handlebars_parser.h"
#include "handlebars_string.h"
#include "handlebars_token.h"

#include "php7to8.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsToken_ce_ptr;
static zend_string *INTERNED_NAME;
static zend_string *INTERNED_TEXT;
/* }}} Variables & Prototypes */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsToken_construct_args, ZEND_SEND_BY_VAL, 0, 2)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

PHP_HANDLEBARS_API void php_handlebars_token_ctor(struct handlebars_token * token, zval * z_token)
{
    zval z_const = {0};
    zval z_ret = {0};
    zval name = {0};
    zval text = {0};

    ZVAL_STRING(&name, (char *) handlebars_token_readable_type(handlebars_token_get_type(token)));

    struct handlebars_string * token_text = handlebars_token_get_text(token);
    if( token_text ) {
        HBS_ZVAL_STR(&text, token_text);
    } else {
        ZVAL_STRINGL(&text, "", 0);
    }

    object_init_ex(z_token, HandlebarsToken_ce_ptr);

    zval z_const_args[2] = {0};

    ZVAL_STRING(&z_const, "__construct");
    z_const_args[0] = name;
    z_const_args[1] = text;

    call_user_function(&HandlebarsToken_ce_ptr->function_table, z_token, &z_const, &z_ret, 2, z_const_args);

    zval_dtor(&z_const);

    zval_ptr_dtor(&name);
    zval_ptr_dtor(&text);
}

/* {{{ proto Handlebars\Token::__construct(string name, string text) */
PHP_METHOD(HandlebarsToken, __construct)
{
    zval * _this_zval = getThis();
    zend_string * name;
    zend_string * text;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(name)
        Z_PARAM_STR(text)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_str(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), "name", sizeof("name")-1, name);
    zend_update_property_str(Z_OBJCE_P(_this_zval), PHP7TO8_Z_OBJ_P(_this_zval), "text", sizeof("text")-1, text);
}
/* }}} Handlebars\Token::__construct */

/* {{{ HandlebarsToken methods */
static zend_function_entry HandlebarsToken_methods[] = {
    PHP_ME(HandlebarsToken, __construct, HandlebarsToken_construct_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} HandlebarsToken methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_token)
{
    zend_class_entry ce;
	zval default_val;

    INTERNED_NAME = zend_new_interned_string(zend_string_init(ZEND_STRL("name"), 1));
    INTERNED_TEXT = zend_new_interned_string(zend_string_init(ZEND_STRL("text"), 1));

    INIT_CLASS_ENTRY(ce, "Handlebars\\Token", HandlebarsToken_methods);
    HandlebarsToken_ce_ptr = zend_register_internal_class(&ce);

#if PHP_VERSION_ID >= 70400
	ZVAL_UNDEF(&default_val);
#endif

// Current TravisCI's PHP master is really old, falling through to PHP 7.4 case should fix it...
#if PHP_VERSION_ID >= 80000 && defined(ZEND_TYPE_INIT_CODE)

	ZVAL_UNDEF(&default_val);
	zend_declare_typed_property(HandlebarsToken_ce_ptr, INTERNED_NAME, &default_val, ZEND_ACC_PUBLIC, NULL,
            (zend_type) ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0));
	zend_declare_typed_property(HandlebarsToken_ce_ptr, INTERNED_TEXT, &default_val, ZEND_ACC_PUBLIC, NULL,
            (zend_type) ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0));

#elif PHP_VERSION_ID >= 70400

	ZVAL_UNDEF(&default_val);
	zend_declare_typed_property(HandlebarsToken_ce_ptr, INTERNED_NAME, &default_val, ZEND_ACC_PUBLIC, NULL,
            ZEND_TYPE_ENCODE(IS_STRING, 0));
	zend_declare_typed_property(HandlebarsToken_ce_ptr, INTERNED_TEXT, &default_val, ZEND_ACC_PUBLIC, NULL,
            ZEND_TYPE_ENCODE(IS_STRING, 0));

#else

	ZVAL_NULL(&default_val);
    zend_declare_property_ex(HandlebarsToken_ce_ptr, INTERNED_NAME, &default_val, ZEND_ACC_PUBLIC, NULL);
    zend_declare_property_ex(HandlebarsToken_ce_ptr, INTERNED_TEXT, &default_val, ZEND_ACC_PUBLIC, NULL);

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
