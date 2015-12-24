
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_exceptions.h"
#include "main/php.h"
#include "ext/spl/spl_exceptions.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsException_ce_ptr;
zend_class_entry * HandlebarsLexException_ce_ptr;
zend_class_entry * HandlebarsParseException_ce_ptr;
zend_class_entry * HandlebarsCompileException_ce_ptr;
zend_class_entry * HandlebarsInvalidArgumentException_ce_ptr;
zend_class_entry * HandlebarsRuntimeException_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_exceptions)
{
    zend_class_entry ce;
    zend_class_entry * exception_ce = zend_exception_get_default(TSRMLS_C);

    // Handlebars\Exception
    INIT_CLASS_ENTRY(ce, "Handlebars\\Exception", NULL);
    HandlebarsException_ce_ptr = zend_register_internal_interface(&ce TSRMLS_CC);

    // Handlebars\LexException
    INIT_CLASS_ENTRY(ce, "Handlebars\\LexException", NULL);
    HandlebarsLexException_ce_ptr = php5to7_register_internal_class_ex(&ce, exception_ce);
    zend_class_implements(HandlebarsLexException_ce_ptr TSRMLS_CC, 1, HandlebarsException_ce_ptr);

    // Handlebars\ParseException
    INIT_CLASS_ENTRY(ce, "Handlebars\\ParseException", NULL);
    HandlebarsParseException_ce_ptr = php5to7_register_internal_class_ex(&ce, exception_ce);
    zend_class_implements(HandlebarsParseException_ce_ptr TSRMLS_CC, 1, HandlebarsException_ce_ptr);

    // Handlebars\CompileException
    INIT_CLASS_ENTRY(ce, "Handlebars\\CompileException", NULL);
    HandlebarsCompileException_ce_ptr = php5to7_register_internal_class_ex(&ce, exception_ce);
    zend_class_implements(HandlebarsCompileException_ce_ptr TSRMLS_CC, 1, HandlebarsException_ce_ptr);

    // Handlebars\InvalidArgumentException
    INIT_CLASS_ENTRY(ce, "Handlebars\\InvalidArgumentException", NULL);
    HandlebarsInvalidArgumentException_ce_ptr = php5to7_register_internal_class_ex(&ce, spl_ce_RuntimeException);
    zend_class_implements(HandlebarsInvalidArgumentException_ce_ptr TSRMLS_CC, 1, HandlebarsException_ce_ptr);

    // Handlebars\RuntimeException
    INIT_CLASS_ENTRY(ce, "Handlebars\\RuntimeException", NULL);
    HandlebarsRuntimeException_ce_ptr = php5to7_register_internal_class_ex(&ce, spl_ce_RuntimeException);
    zend_class_implements(HandlebarsRuntimeException_ce_ptr TSRMLS_CC, 1, HandlebarsException_ce_ptr);

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
