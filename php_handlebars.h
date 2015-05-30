
/* vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab */

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#ifndef PHP_HANDLEBARS_H
#define PHP_HANDLEBARS_H

#define PHP_HANDLEBARS_NAME "handlebars"
#define PHP_HANDLEBARS_VERSION "0.3.1"
#define PHP_HANDLEBARS_RELEASE "2015-05-30"
#define PHP_HANDLEBARS_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"
#define PHP_HANDLEBARS_SPEC "2.0.0"

extern zend_module_entry handlebars_module_entry;
#define phpext_handlebars_ptr &handlebars_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(handlebars)
    char * handlebars_last_error;
ZEND_END_MODULE_GLOBALS(handlebars)

//ZEND_EXTERN_MODULE_GLOBALS(handlebars);

#ifdef ZTS
#define HANDLEBARS_G(v) TSRMG(handlebars_globals_id, zend_handlebars_globals *, v)
#else
#define HANDLEBARS_G(v) (handlebars_globals.v)
#endif

#endif	/* PHP_HANDLEBARS_H */
