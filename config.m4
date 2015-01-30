
dnl HANDLEBARS -----------------------------------------------------------------
PHP_ARG_ENABLE(handlebars, whether to enable handlebars support,
dnl Make sure that the comment is aligned:
[  --enable-handlebars Enable handlebars support])

dnl MAIN -----------------------------------------------------------------------
if test "$PHP_HANDLEBARS" != "no"; then
  PHP_ADD_LIBRARY(handlebars, 1, HANDLEBARS_SHARED_LIBADD)
  PHP_NEW_EXTENSION(handlebars, handlebars.c, $ext_shared, , $PHP_HANDLEBARS_FLAGS)
  PHP_SUBST(HANDLEBARS_SHARED_LIBADD)
fi
