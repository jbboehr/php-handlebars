
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"

#include "handlebars.h"
#include "handlebars_context.h"
#include "handlebars_memory.h"
#include "handlebars_token.h"
#include "handlebars_token_list.h"
#include "handlebars_token_printer.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsTokenizer_ce_ptr;
/* }}} Variables & Prototypes */

static zend_always_inline void token_to_zval(struct handlebars_token * token, zval * current TSRMLS_DC)
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

	object_init_ex(current, HandlebarsToken_ce_ptr);

	do {
#if PHP_MAJOR_VERSION < 7
		zval **z_const_args = emalloc(2 * sizeof(zval *));

		ZVAL_STRING(&z_const, "__construct", 0);
		z_const_args[0] = name;
		z_const_args[1] = text;

	    call_user_function(&HandlebarsToken_ce_ptr->function_table, &current, &z_const, &z_ret, 2, z_const_args TSRMLS_CC);

		efree(z_const_args);
#else
		zval z_const_args[2];

		ZVAL_STRING(&z_const, "__construct");
		z_const_args[0] = *name;
		z_const_args[1] = *text;

		call_user_function(&HandlebarsToken_ce_ptr->function_table, current, &z_const, &z_ret, 2, z_const_args TSRMLS_CC);

		zval_dtor(&z_const);
#endif
	} while(0);

	zval_ptr_dtor(name);
	zval_ptr_dtor(text);
}

/* {{{ proto mixed Handlebars\Tokenizer::lex(string tmpl) */
static zend_always_inline void php_handlebars_lex(INTERNAL_FUNCTION_PARAMETERS, short print)
{
    char * tmpl;
    strsize_t tmpl_len;
    struct handlebars_context * ctx;
    struct handlebars_token_list * list;
    struct handlebars_token_list_item * el = NULL;
    struct handlebars_token_list_item * tmp = NULL;
    char * output;
    _DECLARE_ZVAL(child);

#ifndef FAST_ZPP
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tmpl, &tmpl_len) == FAILURE ) {
        return;
    }
#else
    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STRING(tmpl, tmpl_len)
    ZEND_PARSE_PARAMETERS_END();
#endif

    ctx = handlebars_context_ctor();
    ctx->tmpl = tmpl;
    list = handlebars_lex(ctx);

    if( print ) {
        output = handlebars_token_list_print(list, 0);
        PHP5TO7_RETVAL_STRING(output);
    } else {
        array_init(return_value);
        handlebars_token_list_foreach(list, el, tmp) {
            _ALLOC_INIT_ZVAL(child);
            token_to_zval(el->data, child TSRMLS_CC);
            add_next_index_zval(return_value, child);
        }
    }

    handlebars_context_dtor(ctx);
}

PHP_METHOD(HandlebarsTokenizer, lex)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_METHOD(HandlebarsTokenizer, lexPrint)
{
    php_handlebars_lex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} Handlebars\Tokenizer::lex */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsTokenizer_lex_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsTokenizer methods */
static zend_function_entry HandlebarsTokenizer_methods[] = {
    PHP_ME(HandlebarsTokenizer, lex, HandlebarsTokenizer_lex_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsTokenizer, lexPrint, HandlebarsTokenizer_lex_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  { NULL, NULL, NULL }
};
/* }}} HandlebarsTokenizer methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_tokenizer)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Tokenizer", HandlebarsTokenizer_methods);
    HandlebarsTokenizer_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

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
