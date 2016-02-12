
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "main/php.h"




#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsVM_ce_ptr;
/* }}} Variables & Prototypes */

PHP_METHOD(HandlebarsVM, registerHelper)
{

}

PHP_METHOD(HandlebarsVM, registerHelpers)
{

}

PHP_METHOD(HandlebarsVM, registerPartial)
{

}

PHP_METHOD(HandlebarsVM, registerPartials)
{

}

PHP_METHOD(HandlebarsVM, render)
{

}

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_registerHelper_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, helper)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_registerHelpers_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, helpers, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_registerPartial_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, helper)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_registerPartials_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_ARRAY_INFO(0, partials, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
    ZEND_ARG_INFO(0, context)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsVM methods */
static zend_function_entry HandlebarsCompiler_methods[] = {
    PHP_ME(HandlebarsVM, registerHelper, HandlebarsVM_registerHelper_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, registerHelpers, HandlebarsVM_registerHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, registerPartial, HandlebarsVM_registerPartial_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, registerPartials, HandlebarsVM_registerPartials_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, render, HandlebarsVM_render_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} HandlebarsVM methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_vm)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;

    INIT_CLASS_ENTRY(ce, "Handlebars\\VM", HandlebarsCompiler_methods);
    HandlebarsVM_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

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
