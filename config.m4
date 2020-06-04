
# vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab

m4_include(m4/ax_cflags_warn_all.m4)
m4_include(m4/ax_add_fortify_source.m4)
m4_include(m4/ax_require_defined.m4)
m4_include(m4/ax_append_flag.m4)
m4_include(m4/ax_append_compile_flags.m4)
m4_include(m4/ax_append_link_flags.m4)
m4_include(m4/ax_check_compile_flag.m4)
m4_include(m4/ax_check_link_flag.m4)
m4_include(m4/ax_compiler_flags_cflags.m4)
m4_include(m4/ax_compiler_flags_ldflags.m4)
m4_include(m4/ax_compiler_flags_gir.m4)
m4_include(m4/ax_compiler_flags.m4)
m4_define(AM_LDFLAGS, [LDFLAGS])

# args
PHP_ARG_ENABLE(handlebars, whether to enable handlebars support,
[  --enable-handlebars     Enable handlebars support])

PHP_ARG_ENABLE(handlebars-psr, whether to enable handlebars PSR support,
[  --enable-handlebars-psr Enable handlebars PSR support], [no], [no])

PHP_ARG_ENABLE(handlebars-hardening, whether to enable handlebars hardening support,
[  --enable-handlebars-hardening Enable handlebars hardening support], [yes], [no])

AC_DEFUN([PHP_HANDLEBARS_ADD_SOURCES], [
  PHP_HANDLEBARS_SOURCES="$PHP_HANDLEBARS_SOURCES $1"
])

# main
if test "$PHP_HANDLEBARS" != "no"; then
    # compilers
    AC_PROG_CC_STDC
    AC_PROG_CC_C99

    AC_MSG_CHECKING([if compiling with gcc])
    AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([], [[
    #ifndef __GNUC__
        not gcc
    #endif
    ]])],
    [GCC=yes], [GCC=no])
    AC_MSG_RESULT([$GCC])

    AC_MSG_CHECKING([if compiling with clang])
    AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([], [[
    #ifndef __clang__
        not clang
    #endif
    ]])],
    [CLANG=yes], [CLANG=no])
    AC_MSG_RESULT([$CLANG])

    # programs
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

    # warnings
    AX_CFLAGS_WARN_ALL([WARN_CFLAGS])
    AX_COMPILER_FLAGS([WARN_CFLAGS],[WARN_LDFLAGS],,,[ \
        -Wdeprecated-declarations \
        -Wmaybe-uninitialized \
        -Wno-declaration-after-statement -Wno-error=declaration-after-statement \
        -Wno-missing-include-dirs -Wno-error=missing-include-dirs \
        -Wno-undef -Wno-error=undef \
        -Wno-strict-prototypes -Wno-error=strict-prototypes \
        -Wno-old-style-definition -Wno-error=old-style-definition \
        -Wno-missing-prototypes -Wno-error=missing-prototypes \
        -Wno-missing-declarations -Wno-error=missing-declarations \
        -Wno-redundant-decls -Wno-error=redundant-decls \
        -Wno-jump-misses-init -Wno-error=jump-misses-init \
        -Wno-clobbered -Wno-error=clobbered \
    ])
    CFLAGS="$WARN_CFLAGS $CFLAGS"
    LDFLAGS="$WARN_LDFLAGS $LDFLAGS"

    # hardening
    if test "$PHP_HANDLEBARS_HARDENING" != "no"; then
        AX_ADD_FORTIFY_SOURCE()
        AX_APPEND_COMPILE_FLAGS([-fstack-protector-strong -fstack-clash-protection], [HARDENING_CFLAGS])
        AX_APPEND_LINK_FLAGS([-fstack-protector-strong -fstack-clash-protection], [HARDENING_LDFLAGS])
        # See: https://github.com/gdnsd/gdnsd/blob/f23273c688ae267d2b6049f0608f7b42193211fe/configure.ac#L84
        AS_IF([test "x$CLANG" != "xyes"], [
            AX_APPEND_COMPILE_FLAGS([-fcf-protection], [HARDENING_CFLAGS])
            AX_APPEND_LINK_FLAGS([-fcf-protection], [HARDENING_LDFLAGS])
        ])
        # fpic
        AX_APPEND_COMPILE_FLAGS([-fpic], [HARDENING_CFLAGS])
        AX_APPEND_LINK_FLAGS([-fpic], [HARDENING_LDFLAGS])
    fi
    CFLAGS="$HARDENING_CFLAGS $CFLAGS"
    LDFLAGS="$HARDENING_CFLAGS $LDFLAGS"

    # libhandlebars
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

    # extension
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

