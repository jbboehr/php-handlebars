
# vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab

# HANDLEBARS -------------------------------------------------------------------
PHP_ARG_ENABLE(handlebars, whether to enable handlebars support,
# Make sure that the comment is aligned:
[  --enable-handlebars Enable handlebars support])

AC_DEFUN([PHP_HANDLEBARS_ADD_SOURCES], [
  PHP_HANDLEBARS_SOURCES="$PHP_HANDLEBARS_SOURCES $1"
])

# MAIN -------------------------------------------------------------------------
if test "$PHP_HANDLEBARS" != "no"; then
	PHP_HANDLEBARS_ADD_SOURCES([
		handlebars.c
		handlebars_compiler.c
		handlebars_exceptions.c
		handlebars_parser.c
		handlebars_safe_string.c
		handlebars_tokenizer.c
		handlebars_utils.c
	])
    PHP_INSTALL_HEADERS([ext/handlebars], [php_handlebars.h])
    PHP_ADD_LIBRARY(handlebars, 1, HANDLEBARS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(handlebars, $PHP_HANDLEBARS_SOURCES, $ext_shared, , $PHP_HANDLEBARS_FLAGS)
    PHP_SUBST(HANDLEBARS_SHARED_LIBADD)
fi

