
#ifndef PHP_HANDLEBARS_H
#define PHP_HANDLEBARS_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_HANDLEBARS_NAME "handlebars"
#define PHP_HANDLEBARS_VERSION "0.5.2"
#define PHP_HANDLEBARS_RELEASE "2015-09-22"
#define PHP_HANDLEBARS_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"
#define PHP_HANDLEBARS_SPEC "3.0.3"

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */

