
#if PHP_MAJOR_VERSION >= 8
#define PHP7TO8_Z_OBJ_P(zv) Z_OBJ_P(zv)
#define php7to8_escape_html_entities_ex(a, b, c, d, e, f) php_escape_html_entities_ex(a, b, c, d, e, f, 0)
#else
#define PHP7TO8_Z_OBJ_P(zv) (zv)
#define php7to8_escape_html_entities_ex(a, b, c, d, e, f) php_escape_html_entities_ex(a, b, c, d, e, f)
#endif
