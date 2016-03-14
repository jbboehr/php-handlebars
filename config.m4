
# vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab

# HANDLEBARS -------------------------------------------------------------------
PHP_ARG_ENABLE(handlebars, whether to enable handlebars support,
[  --enable-handlebars     Enable handlebars support])

PHP_ARG_ENABLE(handlebars-psr, whether to enable handlebars support,
[  --enable-handlebars-psr Enable handlebars PSR support])

AC_DEFUN([PHP_HANDLEBARS_ADD_SOURCES], [
  PHP_HANDLEBARS_SOURCES="$PHP_HANDLEBARS_SOURCES $1"
])

# MAIN -------------------------------------------------------------------------
if test "$PHP_HANDLEBARS" != "no"; then
	PHP_HANDLEBARS_ADD_SOURCES([
		php_handlebars.c
		impl.c
		compile_context.c
		compiler.c
		exceptions.c
		opcode.c
		options.c
		parser.c
		registry.c
		safe_string.c
		token.c
		tokenizer.c
		utils.c
		vm.c
		value.c
	])
    PHP_INSTALL_HEADERS([ext/handlebars], [php_handlebars.h])
    PHP_ADD_LIBRARY(handlebars, 1, HANDLEBARS_SHARED_LIBADD)

    if test "$PHP_HANDLEBARS_PSR" != "no"; then
        ifdef([PHP_CHECK_PSR_INCLUDES], [PHP_CHECK_PSR_INCLUDES],[
            AC_MSG_CHECKING([for PSR includes])
            if test -f $abs_srcdir/include/php/ext/psr/php_psr.h; then
              psr_cv_inc_path=$abs_srcdir/ext
            elif test -f $abs_srcdir/ext/psr/php_psr.h; then
              psr_cv_inc_path=$abs_srcdir/ext
            elif test -f $phpincludedir/ext/psr/php_psr.h; then
              psr_cv_inc_path=$phpincludedir/ext
            else
              AC_MSG_ERROR([Cannot find php_psr.h.])
            fi
            AC_MSG_RESULT($psr_cv_inc_path)
        ])
        AC_DEFINE(HAVE_HANDLEBARS_PSR,1,[Whether to build Handlebars with PSR support])
        ifdef([PHP_ADD_EXTENSION_DEP], [
            PHP_ADD_EXTENSION_DEP(handlebars, psr)
        ])
    fi

    PHP_NEW_EXTENSION(handlebars, $PHP_HANDLEBARS_SOURCES, $ext_shared, , -I$psr_cv_inc_path $PHP_HANDLEBARS_FLAGS)
    PHP_SUBST(HANDLEBARS_SHARED_LIBADD)
fi
