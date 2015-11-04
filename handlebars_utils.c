
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"
#include "ext/standard/html.h"
#include "ext/standard/php_array.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsUtils_ce_ptr;
/* }}} Variables & Prototypes */

/* {{{ proto mixed Handlebars\Utils::appendContextPath(mixed contextPath, string id) */
PHP_METHOD(HandlebarsUtils, appendContextPath)
{
    zval * context_path;
    char * id;
    strsize_t id_length;
    zval * entry = NULL;
    zval ** entry2 = NULL;
    char * tmp = NULL;
    strsize_t tmp_length = 0;
    char * out;

    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &context_path, &id, &id_length) == FAILURE ) {
        return;
    }

    switch( Z_TYPE_P(context_path) ) {
        case IS_ARRAY:
        	if( (entry = php5to7_zend_hash_find(HASH_OF(context_path), "contextPath", sizeof("contextPath") - 1)) ) {
                if( Z_TYPE_P(entry) == IS_STRING ) {
                    tmp = Z_STRVAL_P(entry);
                    tmp_length = Z_STRLEN_P(entry);
                }
        	}
            break;
        case IS_OBJECT:
#if PHP_MAJOR_VERSION < 7
            entry = zend_read_property(Z_OBJCE_P(context_path), context_path, "contextPath", sizeof("contextPath") - 1, 1 TSRMLS_CC);
#else
            entry = zend_read_property(Z_OBJCE_P(context_path), context_path, "contextPath", sizeof("contextPath") - 1, 1, NULL);
#endif
            if( entry && Z_TYPE_P(entry) == IS_STRING ) {
                tmp = Z_STRVAL_P(entry);
                tmp_length = Z_STRLEN_P(entry);
            }
            break;
        case IS_STRING:
            tmp = Z_STRVAL_P(context_path);
            tmp_length = Z_STRLEN_P(context_path);
            break;
    }

    if( tmp != NULL && tmp_length > 0 ) {
        spprintf(&out, 0, "%.*s.%.*s", (int) tmp_length, tmp, (int) id_length, id);
        PHP5TO7_RETVAL_STRING(out);
        efree(out);
    } else {
    	PHP5TO7_RETVAL_STRING(id);
    }
}
/* }}} Handlebars\Utils::appendContextPath */

/* {{{ proto mixed Handlebars\Utils::createFrame(mixed $value) */
#if PHP_MAJOR_VERSION < 7
static inline void php_handlebars_create_frame(zval * return_value, zval * value, zval * frame TSRMLS_DC)
{
    array_init(return_value);

    switch( Z_TYPE_P(value) ) {
        case IS_ARRAY:
            php_array_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_P(value), 1 TSRMLS_CC);
            add_assoc_zval_ex(return_value, PHP5TO7_STRL("_parent"), value);
            break;
        default:
            add_next_index_zval(return_value, value);
            break;
    }

    // @todo is this necessary?
    zval_copy_ctor(return_value);
}
#else
static inline void php_handlebars_create_frame(zval * return_value, zval * value, zval * frame)
{
    zval tmp;

    switch( Z_TYPE_P(value) ) {
        case IS_ARRAY:
            array_init(return_value);
            php_array_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_P(value));
            ZVAL_COPY(&tmp, value);
            add_assoc_zval_ex(return_value, PHP5TO7_STRL("_parent"), &tmp);
            break;
        default:
            array_init(return_value);
            ZVAL_COPY(&tmp, value);
            add_next_index_zval(return_value, &tmp);
            break;
    }
}
#endif

PHP_METHOD(HandlebarsUtils, createFrame)
{
    zval * value;
    zval * frame;

    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE ) {
        return;
    }

    php_handlebars_create_frame(return_value, value, frame TSRMLS_CC);
}
/* }}} Handlebars\Utils::createFrame */

/* {{{ proto mixed Handlebars\Utils::nameLookup(mixed value, string field) */
static zend_always_inline short is_integer_string(char * str, strsize_t len) {
    char * endstr = str + len;
    for( ; str != endstr; str++ ) {
        if( !ZEND_IS_DIGIT(*str) ) {
            return 0;
        }
    }
    return 1;
}
#if PHP_MAJOR_VERSION < 7
static zend_always_inline void php_handlebars_name_lookup(zval * value, zval * field, zval * return_value TSRMLS_DC)
{
    long index = -1;
    HashTable * data_hash;
    zval * entry = NULL;
    zval * fname;
    zval * prop;
    zval * params[1];

    // Support integer keys
    switch( Z_TYPE_P(field) ) {
        case IS_LONG:
            index = Z_LVAL_P(field);
            convert_to_string(field);
            break;
        default:
            convert_to_string(field);
            // fall-through
        case IS_STRING:
            if( is_integer_string(Z_STRVAL_P(field), Z_STRLEN_P(field)) ) {
                sscanf(Z_STRVAL_P(field), "%ld", &index);
            }
            break;
    }

    switch( Z_TYPE_P(value) ) {
        case IS_ARRAY:
            if( index > -1 && (entry = php5to7_zend_hash_index_find(Z_ARRVAL_P(value), index)) ) {

            } else {
            	entry = php5to7_zend_hash_find(Z_ARRVAL_P(value), Z_STRVAL_P(field), Z_STRLEN_P(field));
            }
            break;
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(value), zend_ce_arrayaccess TSRMLS_CC) ) {
                MAKE_STD_ZVAL(prop);
                ZVAL_STRINGL(prop, Z_STRVAL_P(field), Z_STRLEN_P(field), 1);
                if( Z_OBJ_HT_P(value)->has_dimension(value, prop, 0 TSRMLS_CC) ) {
                	entry = Z_OBJ_HT_P(value)->read_dimension(value, prop, 0 TSRMLS_CC);
                }
                zval_ptr_dtor(&prop);
            } else {
            	entry = zend_read_property(Z_OBJCE_P(value), value, Z_STRVAL_P(field), Z_STRLEN_P(field), 1 TSRMLS_CC);
            }
            break;
    }

    if( entry ) {
        RETVAL_ZVAL(entry, 1, 0);
    }
}
#else
static zend_always_inline void php_handlebars_name_lookup(zval * value, zval * field, zval * return_value TSRMLS_DC)
{
    long long_index;
    zend_long index = -1;
    zval * entry = NULL;
    zval fname;
    zval prop;
    HashTable * data_hash;
    zval result = {0};
    zval *retval = NULL;

    // Support integer keys
    switch( Z_TYPE_P(field) ) {
        case IS_LONG:
            index = Z_LVAL_P(field);
            convert_to_string(field);
            break;
        default:
            convert_to_string(field);
            // fall-through
        case IS_STRING:
            if( is_integer_string(Z_STRVAL_P(field), Z_STRLEN_P(field)) ) {
                sscanf(Z_STRVAL_P(field), "%ld", &index);
            }
            break;
    }

    switch( Z_TYPE_P(value) ) {
        case IS_ARRAY:
            if( index > -1 && (entry = php5to7_zend_hash_index_find(Z_ARRVAL_P(value), index)) ) {
                // nothing
            } else {
                entry = php5to7_zend_hash_find(Z_ARRVAL_P(value), Z_STRVAL_P(field), Z_STRLEN_P(field));
            }
            break;
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(value), zend_ce_arrayaccess TSRMLS_CC) ) {
                if( Z_OBJ_HT_P(value)->has_dimension(value, field, 0 TSRMLS_CC) ) {
                    retval = Z_OBJ_HT_P(value)->read_dimension(value, field, 0, &result TSRMLS_CC);
                    if( retval ) {
                        if( &result != retval ) {
                            ZVAL_COPY(&result, retval);
                        }
                    } else {
                        ZVAL_NULL(&result);
                    }
                    RETVAL_ZVAL(&result, 0, 0);
                }
            } else {
                entry = zend_read_property(Z_OBJCE_P(value), value, Z_STRVAL_P(field), Z_STRLEN_P(field), 1, NULL TSRMLS_CC);
            }
            break;
    }

    if( entry ) {
        if( Z_TYPE_P(entry) == IS_INDIRECT ) {
            entry = Z_INDIRECT_P(entry);
        }
        RETVAL_ZVAL(entry, 1, 0);
    }
}
#endif

PHP_METHOD(HandlebarsUtils, nameLookup)
{
    zval * value;
    zval * field;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &value, &field) == FAILURE ) {
        return;
    }

    php_handlebars_name_lookup(value, field, return_value TSRMLS_CC);
}
/* }}} Handlebars\Utils::nameLookup */

/* {{{ proto boolean Handlebars\Utils::isCallable(mixed name) */
PHP_METHOD(HandlebarsUtils, isCallable)
{
    zval * var;
    char * error;
    zend_bool retval = 0;
    int check_flags = 0; //IS_CALLABLE_CHECK_SYNTAX_ONLY;

    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &var) == FAILURE ) {
        return;
    }

    if( Z_TYPE_P(var) != IS_OBJECT ) {
        RETURN_FALSE;
    }

#if PHP_MAJOR_VERSION < 7
    retval = zend_is_callable_ex(var, NULL, check_flags, NULL, NULL, NULL, &error TSRMLS_CC);
#else
    retval = zend_is_callable_ex(var, NULL, check_flags, NULL, NULL, &error);
#endif

    if (error) {
        efree(error);
    }

    RETURN_BOOL(retval);
}
/* }}} Handlebars\Utils::isCallable */

/* {{{ proto boolean Handlebars\Utils::isIntArray(mixed value) */
#if PHP_MAJOR_VERSION < 7
static zend_always_inline zend_bool php_handlebars_is_int_array(zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    HashPosition data_pointer = NULL;
    zval ** data_entry = NULL;
    char * key;
    int key_len;
    long index;
    long idx = 0;

    if( Z_TYPE_P(arr) != IS_ARRAY ) {
        return 0;
    }

    data_hash = Z_ARRVAL_P(arr);

    // An empty array is an int array
    if( !zend_hash_num_elements(data_hash) ) {
        return 1;
    }

    zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
    while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
        if (zend_hash_get_current_key_ex(data_hash, &key, &key_len, &index, 0, &data_pointer) == HASH_KEY_IS_STRING) {
            return 0;
        }
        // Make sure the keys are sequential
        if( index != idx++ ) {
            return 0;
        }
        zend_hash_move_forward_ex(data_hash, &data_pointer);
    }

    return 1;
}
#else
static zend_always_inline zend_bool php_handlebars_is_int_array(zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    zend_string * key;
    zend_ulong index;
    zend_ulong idx = 0;

    if( Z_TYPE_P(arr) != IS_ARRAY ) {
        return 0;
    }

    data_hash = Z_ARRVAL_P(arr);

    // An empty array is an int array
    if( !zend_hash_num_elements(data_hash) ) {
        return 1;
    }

    ZEND_HASH_FOREACH_KEY(data_hash, index, key) {
        if( key ) {
            return 0;
        } else {
            if( index != idx ) {
                return 0;
            }
        }
        idx++;
    } ZEND_HASH_FOREACH_END();

    return 1;
}
#endif

PHP_METHOD(HandlebarsUtils, isIntArray)
{
    zval * arr;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arr) == FAILURE ) {
        return;
    }

    if( php_handlebars_is_int_array(arr TSRMLS_CC) ) {
        RETURN_TRUE;
    } else  {
        RETURN_FALSE;
    }
}
/* }}} Handlebars\Utils::isIntArray */

/* {{{ proto string Handlebars\Utils::expression(mixed value) */
#if PHP_MAJOR_VERSION < 7
static zend_always_inline zend_bool php_handlebars_expression(zval * val, zval * return_value TSRMLS_DC)
{
    zval delim;

    switch( Z_TYPE_P(val) ) {
        case IS_BOOL:
        	PHP5TO7_RETVAL_STRING(Z_BVAL_P(val) ? "true" : "false");
            break;
        case IS_ARRAY:
            if( php_handlebars_is_int_array(val TSRMLS_CC) ) {
                ZVAL_STRING(&delim, ",", 0);
                php_implode(&delim, val, return_value TSRMLS_CC);
            } else {
                zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Trying to stringify assoc array", 0 TSRMLS_CC);
                return 0;
            }
            break;
        case IS_OBJECT:
            if( !zend_hash_exists(&Z_OBJCE_P(val)->function_table, "__tostring", sizeof("__toString")) ) {
                zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Trying to stringify object", 0 TSRMLS_CC);
                return 0;
            }
            // fall through
        default:
            convert_to_string(val);
            RETVAL_ZVAL(val, 1, 0);
            break;
    }

    return 1;
}
#else
static zend_always_inline zend_bool php_handlebars_expression(zval * val, zval * return_value TSRMLS_DC)
{
    zend_string * delim;

    switch( Z_TYPE_P(val) ) {
        case IS_TRUE:
        	PHP5TO7_RETVAL_STRING("true");
            break;
        case IS_FALSE:
        	PHP5TO7_RETVAL_STRING("false");
            break;
        case IS_ARRAY:
            if( php_handlebars_is_int_array(val TSRMLS_CC) ) {
                delim = zend_string_init(",", 1, 0);
                php_implode(delim, val, return_value TSRMLS_CC);
                zend_string_free(delim);
            } else {
                zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Trying to stringify assoc array", 0 TSRMLS_CC);
                return 0;
            }
            break;
        case IS_OBJECT:
            if( !zend_hash_str_exists(&Z_OBJCE_P(val)->function_table, "__tostring", sizeof("__toString") - 1) ) {
                zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Trying to stringify object", 0 TSRMLS_CC);
                return 0;
            }
            // fall through
        default:
            convert_to_string(val);
            RETVAL_ZVAL(val, 1, 0);
            break;
    }

    return 1;
}
#endif

PHP_METHOD(HandlebarsUtils, expression)
{
    zval * val;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }

    php_handlebars_expression(val, return_value TSRMLS_CC);
}
/* }}} Handlebars\Utils::expression */

/* {{{ proto string Handlebars\Utils::escapeExpression(mixed value) */
#if PHP_MAJOR_VERSION < 7
static zend_always_inline void php_handlebars_escape_expression(zval * val, zval * return_value TSRMLS_DC)
{
    size_t new_len;
    char * replaced;
    zval tmp;

    // @todo this should probably support inheritance
    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1 TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    convert_to_string(val);
    replaced = php_escape_html_entities_ex(Z_STRVAL_P(val), Z_STRLEN_P(val), &new_len, 0, (int) ENT_QUOTES, "UTF-8", 1 TSRMLS_CC);
    RETURN_STRING(replaced, 0);
}
#else
static zend_always_inline void php_handlebars_escape_expression(zval * val, zval * return_value TSRMLS_DC)
{
    zend_string * replaced;
    zval tmp;
    zval rv;

    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1, &rv TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    convert_to_string(val);
    replaced = php_escape_html_entities_ex(Z_STRVAL_P(val), Z_STRLEN_P(val), 0, (int) ENT_QUOTES, "UTF-8", 1 TSRMLS_CC);
    RETURN_STR(replaced);
}
#endif

PHP_METHOD(HandlebarsUtils, escapeExpression)
{
    zval * val;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }

    php_handlebars_escape_expression(val, return_value TSRMLS_CC);
}
/* }}} Handlebars\Utils::escapeExpression */

/* {{{ proto string Handlebars\Utils::escapeExpressionCompat(mixed value) */
static zend_always_inline char * php_handlebars_escape_expression_replace_helper(char * input TSRMLS_DC)
{
    char * output;
    char * source;
    char * target;
    char c;
    long occurrences = 0;
    long newlen;
    long oldlen = strlen(input);

    if( !oldlen ) {
        return estrdup(input);
    }

    // Count the ocurrences of ` and '
    for( source = input; (c = *source); source++ ) {
        switch( c ) {
            case '`':
            case '\'':
                occurrences++;
                break;
        }
    }

    if( !occurrences ) {
        return estrdup(input);
    }

    // Each occurence increases length by 5
    newlen = oldlen + (occurrences * 5) + 1;

    // Allocate new string
    output = target = (char *) emalloc(sizeof(char) * newlen);

    // Replace copy
    for( source = input; (c = *source); source++ ) {
        switch( c ) {
            case '`':
                *target++ = '&';
                *target++ = '#';
                *target++ = 'x';
                *target++ = '6';
                *target++ = '0';
                *target++ = ';';
                break;
            case '\'':
                *target++ = '&';
                *target++ = '#';
                *target++ = 'x';
                *target++ = '2';
                *target++ = '7';
                *target++ = ';';
                break;
            default:
                *target++ = c;
                break;
        }
    }
    *target++ = '\0';

    return output;
}

#if PHP_MAJOR_VERSION < 7
static zend_always_inline void php_handlebars_escape_expression_compat(zval * val, zval * return_value TSRMLS_DC)
{
    size_t new_len;
    char * replaced;
    char * replaced2;
    zval tmp;

    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1 TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    INIT_ZVAL(tmp);
    if( !php_handlebars_expression(val, &tmp TSRMLS_CC) ) {
        return;
    }

    replaced = php_escape_html_entities_ex(Z_STRVAL(tmp), Z_STRLEN(tmp), &new_len, 0, (int) ENT_COMPAT, "UTF-8", 1 TSRMLS_CC);
    zval_dtor(&tmp);

    replaced2 = php_handlebars_escape_expression_replace_helper(replaced TSRMLS_CC);
    efree(replaced);

    RETVAL_STRING(replaced2, 0);
}
#else
static zend_always_inline void php_handlebars_escape_expression_compat(zval * val, zval * return_value TSRMLS_DC)
{
    zend_string * replaced;
    char * replaced2;
    zval tmp;
    zval rv;

    // @todo this should probably support inheritance
    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
        zval * value = zend_read_property(Z_OBJCE_P(val), val, "value", sizeof("value")-1, 1, &rv TSRMLS_CC);
        RETURN_ZVAL(value, 1, 0);
    }

    ZVAL_NULL(&tmp);
    if( !php_handlebars_expression(val, &tmp TSRMLS_CC) ) {
        return;
    }

    replaced = php_escape_html_entities_ex(Z_STRVAL(tmp), Z_STRLEN(tmp), 0, (int) ENT_COMPAT, "UTF-8", 1 TSRMLS_CC);
    zval_dtor(&tmp);

    replaced2 = php_handlebars_escape_expression_replace_helper(replaced->val TSRMLS_CC);
    zend_string_free(replaced);

    PHP5TO7_RETVAL_STRING(replaced2);
    efree(replaced2);
}
#endif

PHP_METHOD(HandlebarsUtils, escapeExpressionCompat)
{
    zval * val;

    // Arguments
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE ) {
        return;
    }

    php_handlebars_escape_expression_compat(val, return_value TSRMLS_CC);
}
/* }}} Handlebars\Utils::escapeExpressionCompat */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_appendContextPath_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, contextPath)
    ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_createFrame_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_nameLookup_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, objOrArray)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_isCallable_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_isIntArray_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, arr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_expression_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsUtils methods */
static zend_function_entry HandlebarsUtils_methods[] = {
    PHP_ME(HandlebarsUtils, appendContextPath, HandlebarsUtils_appendContextPath_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, createFrame, HandlebarsUtils_createFrame_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, nameLookup, HandlebarsUtils_nameLookup_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, isCallable, HandlebarsUtils_isCallable_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, isIntArray, HandlebarsUtils_isIntArray_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, expression, HandlebarsUtils_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, escapeExpression, HandlebarsUtils_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, escapeExpressionCompat, HandlebarsUtils_expression_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  { NULL, NULL, NULL }
};
/* }}} HandlebarsUtils methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_utils)
{
    zend_class_entry ce;

    // Handlebars\Utils
    INIT_CLASS_ENTRY(ce, "Handlebars\\Utils", HandlebarsUtils_methods);
    HandlebarsUtils_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
