
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
		php_handlebars.c
		compile_context.c
		compiler.c
		exceptions.c
		opcode.c
		parser.c
		safe_string.c
		token.c
		tokenizer.c
		utils.c
	])
    PHP_INSTALL_HEADERS([ext/handlebars], [php_handlebars.h])
    PHP_ADD_LIBRARY(handlebars, 1, HANDLEBARS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(handlebars, $PHP_HANDLEBARS_SOURCES, $ext_shared, , $PHP_HANDLEBARS_FLAGS)
    PHP_SUBST(HANDLEBARS_SHARED_LIBADD)
fi

