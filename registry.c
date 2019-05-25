
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"
#include "ext/spl/spl_array.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsRegistry_ce_ptr;
PHP_HANDLEBARS_API zend_class_entry * HandlebarsDefaultRegistry_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_registry)
{
    zend_class_entry ce;;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Registry", NULL);
    HandlebarsRegistry_ce_ptr = zend_register_internal_interface(&ce);
    zend_class_implements(HandlebarsRegistry_ce_ptr, 2, zend_ce_arrayaccess, zend_ce_traversable);

    INIT_CLASS_ENTRY(ce, "Handlebars\\DefaultRegistry", NULL);
    HandlebarsDefaultRegistry_ce_ptr = zend_register_internal_class_ex(&ce, spl_ce_ArrayObject);
    zend_class_implements(HandlebarsDefaultRegistry_ce_ptr, 1, HandlebarsRegistry_ce_ptr);

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
