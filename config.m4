
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
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

    AC_MSG_CHECKING([for libhandlebars])
    if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists handlebars; then
        LIBHANDLEBARS_CFLAGS=`$PKG_CONFIG handlebars --cflags`
        LIBHANDLEBARS_LIBS=`$PKG_CONFIG handlebars --libs`
        LIBHANDLEBARS_VERSION=`$PKG_CONFIG handlebars --modversion`
        AC_MSG_RESULT(version $LIBHANDLEBARS_VERSION found using pkg-config)
        PHP_EVAL_LIBLINE($LIBHANDLEBARS_LIBS, HANDLEBARS_SHARED_LIBADD)
        PHP_EVAL_INCLINE($LIBHANDLEBARS_CFLAGS)
    else
        AC_MSG_ERROR([libhandlebars not found])
    fi

	PHP_HANDLEBARS_ADD_SOURCES([
		php_handlebars.c
		impl.c
		compiler.c
		exceptions.c
		opcode.c
		options.c
		parser.c
		program.c
		registry.c
		safe_string.c
		token.c
		tokenizer.c
		utils.c
		vm.c
		value.c
	])
    PHP_INSTALL_HEADERS([ext/handlebars], [php_handlebars.h])
    PHP_NEW_EXTENSION(handlebars, $PHP_HANDLEBARS_SOURCES, $ext_shared)
    if test "$PHP_HANDLEBARS_PSR" != "no"; then
        PHP_ADD_EXTENSION_DEP(handlebars, psr, true)
    fi
    PHP_SUBST(HANDLEBARS_SHARED_LIBADD)
fi

