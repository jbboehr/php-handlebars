
#ifndef PHP_HANDLEBARS_H
#define PHP_HANDLEBARS_H

#include "php.h"

#define PHP_HANDLEBARS_NAME "handlebars"
#define PHP_HANDLEBARS_VERSION "0.6.1"
#define PHP_HANDLEBARS_RELEASE "2015-10-31"
#define PHP_HANDLEBARS_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"
#define PHP_HANDLEBARS_SPEC "4.0.4"

extern zend_module_entry handlebars_module_entry;
#define phpext_handlebars_ptr &handlebars_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define HANDLEBARS_G(v) TSRMG(handlebars_globals_id, zend_handlebars_globals *, v)
#else
#define HANDLEBARS_G(v) (handlebars_globals.v)
#endif

extern zend_class_entry * HandlebarsCompileContext_ce_ptr;
extern zend_class_entry * HandlebarsCompiler_ce_ptr;
extern zend_class_entry * HandlebarsException_ce_ptr;
extern zend_class_entry * HandlebarsLexException_ce_ptr;
extern zend_class_entry * HandlebarsParseException_ce_ptr;
extern zend_class_entry * HandlebarsCompileException_ce_ptr;
extern zend_class_entry * HandlebarsInvalidArgumentException_ce_ptr;
extern zend_class_entry * HandlebarsRuntimeException_ce_ptr;
extern zend_class_entry * HandlebarsParser_ce_ptr;
extern zend_class_entry * HandlebarsOpcode_ce_ptr;
extern zend_class_entry * HandlebarsSafeString_ce_ptr;
extern zend_class_entry * HandlebarsToken_ce_ptr;
extern zend_class_entry * HandlebarsTokenizer_ce_ptr;
extern zend_class_entry * HandlebarsUtils_ce_ptr;

#endif	/* PHP_HANDLEBARS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
