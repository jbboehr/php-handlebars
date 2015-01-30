
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_handlebars.h"

#include "handlebars.h"
#include "handlebars_context.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"


/* {{{ Utils----------------------------------------------------------------- */

#define HANDLEBARS_SAVE_CLEAR_ERROR(hbs) \
	memset(&handlebars_last_errloc, 0, sizeof(YYLTYPE)); \
    if( handlebars_last_error ) { efree(handlebars_last_error); handlebars_last_error = NULL; } \
    if( hbs.error ) { handlebars_last_error = estrdup(hbs.error); } \
    if( hbs.errloc ) { memcpy(&handlebars_last_errloc, hbs.errloc, sizeof(YYLTYPE)); }

static char * handlebars_last_error = NULL;
static YYLTYPE handlebars_last_errloc;

/* }}} ---------------------------------------------------------------------- */
/* {{{ Functions ------------------------------------------------------------ */

PHP_FUNCTION(handlebars_error)
{
	if( handlebars_last_error ) {
		int len = strlen(handlebars_last_error) + 32;
		char * errmsg = emalloc(sizeof(char) * len);
	    snprintf(errmsg, len, "%s on line %d, column %d", handlebars_last_error, handlebars_last_errloc.last_line, handlebars_last_errloc.last_column);
		RETURN_STRING(errmsg, 0);
	}
}

PHP_FUNCTION(handlebars_lex)
{
    
}

PHP_FUNCTION(handlebars_lex_print)
{
    
}

PHP_FUNCTION(handlebars_parse)
{
    
}

PHP_FUNCTION(handlebars_parse_print)
{
    
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Hooks --------------------------------------------------------- */

static PHP_MINIT_FUNCTION(handlebars)
{
  return SUCCESS;
}

static PHP_MINFO_FUNCTION(handlebars)
{
  php_info_print_table_start();
  php_info_print_table_row(2, "Version", PHP_HANDLEBARS_VERSION);
  php_info_print_table_row(2, "Released", PHP_HANDLEBARS_RELEASE);
  php_info_print_table_row(2, "Authors", PHP_HANDLEBARS_AUTHORS);
  php_info_print_table_row(2, "Spec Version", PHP_HANDLEBARS_SPEC);
  php_info_print_table_end();
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Function Entry ------------------------------------------------------- */

zend_function_entry handlebars_functions[] = {
	PHP_FE(handlebars_error, NULL)
	PHP_FE(handlebars_lex, NULL)
	PHP_FE(handlebars_lex_print, NULL)
	PHP_FE(handlebars_parse, NULL)
	PHP_FE(handlebars_parse_print, NULL)
};

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Entry --------------------------------------------------------- */

zend_module_entry handlebars_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_HANDLEBARS_NAME,                /* Name */
	handlebars_functions,               /* Functions */
	PHP_MINIT(handlebars),              /* MINIT */
	NULL,                               /* MSHUTDOWN */
	NULL,                               /* RINIT */
	NULL,                               /* RSHUTDOWN */
	PHP_MINFO(handlebars),              /* MINFO */
	PHP_HANDLEBARS_VERSION,             /* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HANDLEBARS 
  ZEND_GET_MODULE(handlebars)      // Common for all PHP extensions which are build as shared modules  
#endif

/* }}} ---------------------------------------------------------------------- */
