
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_exceptions.h"
#include "main/php.h"

#include "handlebars.h"
#include "handlebars_memory.h"

#include "handlebars_string.h"
#include "handlebars_token.h"

#define BOOLEAN HBS_BOOLEAN
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsTokenizer_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ proto mixed Handlebars\Tokenizer::lex(string tmpl) */
static inline void php_handlebars_lex(INTERNAL_FUNCTION_PARAMETERS, short print)
{
    zend_string * tmpl = NULL;
    struct handlebars_context * ctx;
    struct handlebars_parser * parser;
    struct handlebars_token ** tokens;
    struct handlebars_string * output;
    jmp_buf buf;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_STR(tmpl)
    ZEND_PARSE_PARAMETERS_END();

    ctx = handlebars_context_ctor();

    // Note: there is no flags argument here, so we can't preprocess the delimiters

    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
    parser = handlebars_parser_ctor(ctx);

    // Lex
    parser->tmpl = handlebars_string_ctor(HBSCTX(parser), ZSTR_VAL(tmpl), ZSTR_LEN(tmpl));
    php_handlebars_try(HandlebarsParseException_ce_ptr, parser, &buf);
    tokens = handlebars_lex(parser);

    // Print or convert to zval
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, parser, &buf);
    if( print ) {
        output = handlebars_string_init(HBSCTX(parser), 256);
        for( ; *tokens; tokens++ ) {
            output = handlebars_token_print_append(HBSCTX(parser), output, *tokens, 0);
        }
        output = handlebars_string_rtrim(output, HBS_STRL("\r\n "));
        RETVAL_STRINGL(output->val, output->len);
    } else {
        array_init(return_value);
        for( ; *tokens; tokens++ ) {
            zval child = {0};
            ZVAL_NULL(&child);
            php_handlebars_token_ctor(*tokens, &child);
            add_next_index_zval(return_value, &child);
        }
    }

done:
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
    PHP_FE_END
};
/* }}} HandlebarsTokenizer methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_tokenizer)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Tokenizer", HandlebarsTokenizer_methods);
    HandlebarsTokenizer_ce_ptr = zend_register_internal_class(&ce);

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
