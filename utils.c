
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"
#include "ext/standard/html.h"
#include "ext/standard/php_array.h"
#include "ext/standard/php_string.h"

#include "handlebars.h"
#include "handlebars_memory.h"

#include "handlebars_string.h"
#include "handlebars_utils.h"

#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsUtils_ce_ptr;
static zend_string *INTERNED_CONTEXT_PATH;
static zend_string *INTERNED_VALUE;
/* }}} Variables & Prototypes */

/* {{{ proto mixed Handlebars\Utils::appendContextPath(mixed contextPath, string id) */
PHP_METHOD(HandlebarsUtils, appendContextPath)
{
    zval * context_path;
    zend_string * id = NULL;
    zval * entry = NULL;
    zend_string * tmp = NULL;
    char * out;

    ZEND_PARSE_PARAMETERS_START(2, 2)
	    Z_PARAM_ZVAL(context_path)
        Z_PARAM_STR(id)
    ZEND_PARSE_PARAMETERS_END();

    switch( Z_TYPE_P(context_path) ) {
        case IS_ARRAY:
        	if( (entry = zend_hash_find(HASH_OF(context_path), INTERNED_CONTEXT_PATH)) ) {
                if( Z_TYPE_P(entry) == IS_STRING ) {
                    tmp = Z_STR_P(entry);
                }
        	}
            break;
        case IS_OBJECT:
            entry = zend_read_property_ex(Z_OBJCE_P(context_path), context_path, INTERNED_CONTEXT_PATH, 1, NULL);
            if( entry && Z_TYPE_P(entry) == IS_STRING ) {
                tmp = Z_STR_P(entry);
            }
            break;
        case IS_STRING:
            tmp = Z_STR_P(context_path);
            break;
    }

    if( tmp != NULL && ZSTR_LEN(tmp) > 0 ) {
        spprintf(&out, 0, "%.*s.%.*s", (int) ZSTR_LEN(tmp), ZSTR_VAL(tmp), (int) ZSTR_LEN(id), ZSTR_VAL(id));
        RETVAL_STRING(out);
        efree(out);
    } else {
    	RETVAL_STR(id);
    }
}
/* }}} Handlebars\Utils::appendContextPath */

/* {{{ proto mixed Handlebars\Utils::createFrame(mixed $value) */
static inline void php_handlebars_create_frame(zval * return_value, zval * value)
{
    zval tmp = {0};

    switch( Z_TYPE_P(value) ) {
        case IS_ARRAY:
            array_init(return_value);
            php_array_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_P(value));
            ZVAL_COPY(&tmp, value);
            add_assoc_zval_ex(return_value, ZEND_STRL("_parent"), &tmp);
            break;
        default:
            array_init(return_value);
            ZVAL_COPY(&tmp, value);
            add_next_index_zval(return_value, &tmp);
            break;
    }
}

PHP_METHOD(HandlebarsUtils, createFrame)
{
    zval * value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    php_handlebars_create_frame(return_value, value);
}
/* }}} Handlebars\Utils::createFrame */

/* {{{ proto mixed Handlebars\Utils::nameLookup(mixed value, string field) */
static zend_always_inline short is_integer_string(char * str, size_t len) {
    char * endstr = str + len;
    for( ; str != endstr; str++ ) {
        if( !ZEND_IS_DIGIT(*str) ) {
            return 0;
        }
    }
    return 1;
}

void php_handlebars_name_lookup(zval * value, zval * field, zval * return_value)
{
    long index = -1;
    zval * entry = NULL;
    zval result;
    zval *retval = NULL;

    ZVAL_UNDEF(&result);

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
            if( index > -1 && (entry = zend_hash_index_find(Z_ARRVAL_P(value), (zend_long) index)) ) {
                // nothing
            } else {
                entry = zend_hash_find(Z_ARRVAL_P(value), Z_STR_P(field));
            }
            break;
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(value), zend_ce_arrayaccess) ) {
#if PHP_MAJOR_VERSION >= 8
                if( Z_OBJ_HT_P(value)->has_dimension(Z_OBJ_P(value), field, 0) ) {
                    retval = Z_OBJ_HT_P(value)->read_dimension(Z_OBJ_P(value), field, 0, &result);
#else
                if( Z_OBJ_HT_P(value)->has_dimension(value, field, 0) ) {
                    retval = Z_OBJ_HT_P(value)->read_dimension(value, field, 0, &result);
#endif
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
                entry = zend_read_property_ex(Z_OBJCE_P(value), value, Z_STR_P(field), 1, NULL);
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

PHP_METHOD(HandlebarsUtils, nameLookup)
{
    zval * value;
    zval * field;

    ZEND_PARSE_PARAMETERS_START(2, 2)
	    Z_PARAM_ZVAL(value)
	    Z_PARAM_ZVAL(field)
    ZEND_PARSE_PARAMETERS_END();

    php_handlebars_name_lookup(value, field, return_value);
}
/* }}} Handlebars\Utils::nameLookup */

/* {{{ proto boolean Handlebars\Utils::indent(string str, string indent) */
PHP_METHOD(HandlebarsUtils, indent)
{
    zend_string * str;
    zend_string * indent;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STR(str)
        Z_PARAM_STR(indent)
    ZEND_PARSE_PARAMETERS_END();

    struct handlebars_string * tmp = handlebars_string_indent(HANDLEBARS_G(context), ZSTR_VAL(str), ZSTR_LEN(str), ZSTR_VAL(indent), ZSTR_LEN(indent));
    RETVAL_STRINGL(tmp->val, tmp->len);
    handlebars_talloc_free(tmp);

}
/* }}} Handlebars\Utils::indent */

/* {{{ proto boolean Handlebars\Utils::isCallable(mixed name) */
PHP_HANDLEBARS_API zend_bool php_handlebars_is_callable(zval * var)
{
    zend_bool retval = 0;
    int check_flags = 0; //IS_CALLABLE_CHECK_SYNTAX_ONLY;
    char * error;

    if( Z_TYPE_P(var) != IS_OBJECT ) {
        return 0;
    }

    retval = zend_is_callable_ex(var, NULL, check_flags, NULL, NULL, &error);

    if (error) {
        efree(error);
    }

    return retval;
}

PHP_METHOD(HandlebarsUtils, isCallable)
{
    zval * var;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_ZVAL(var)
    ZEND_PARSE_PARAMETERS_END();

    RETURN_BOOL(php_handlebars_is_callable(var));
}
/* }}} Handlebars\Utils::isCallable */

/* {{{ proto boolean Handlebars\Utils::isIntArray(array value) */
PHP_HANDLEBARS_API zend_bool php_handlebars_is_int_array(zval *zarr)
{
    zend_string * key;
    zend_ulong index;
    zend_ulong idx = 0;
    HashTable *arr;

    if (Z_TYPE_P(zarr) != IS_ARRAY) {
        return 0;
    }

    arr = Z_ARRVAL_P(zarr);

    // An empty array is an int array
    if( !zend_hash_num_elements(arr) ) {
        return 1;
    }

    ZEND_HASH_FOREACH_KEY(arr, index, key) {
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

PHP_METHOD(HandlebarsUtils, isIntArray)
{
    zval *arr;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_ZVAL(arr)
    ZEND_PARSE_PARAMETERS_END();

    if( php_handlebars_is_int_array(arr) ) {
        RETURN_TRUE;
    } else  {
        RETURN_FALSE;
    }
}
/* }}} Handlebars\Utils::isIntArray */

/* {{{ proto string Handlebars\Utils::expression(mixed value) */
static zend_always_inline zend_bool php_handlebars_expression(zval * val, zval * return_value)
{
    zend_string * delim;

    switch( Z_TYPE_P(val) ) {
        case IS_TRUE:
        	RETVAL_STRING("true");
            break;
        case IS_FALSE:
        	RETVAL_STRING("false");
            break;
        case IS_ARRAY:
            if( php_handlebars_is_int_array(val) ) {
                delim = zend_string_init(",", 1, 0);
#if PHP_VERSION_ID >= 80000
                php_implode(delim, Z_ARRVAL_P(val), return_value);
#else
                php_implode(delim, val, return_value);
#endif
                zend_string_free(delim);
            } else {
                zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Trying to stringify assoc array", 0);
                return 0;
            }
            break;
        case IS_OBJECT:
            if( !zend_hash_str_exists(&Z_OBJCE_P(val)->function_table, "__tostring", sizeof("__toString") - 1) ) {
                zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Trying to stringify object", 0);
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

PHP_METHOD(HandlebarsUtils, expression)
{
    zval * val;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_ZVAL(val)
    ZEND_PARSE_PARAMETERS_END();

    php_handlebars_expression(val, return_value);
}
/* }}} Handlebars\Utils::expression */

/* {{{ proto string Handlebars\Utils::escapeExpression(mixed value) */
static zend_always_inline void php_handlebars_escape_expression(zval * val, zval * return_value)
{
    zend_string * replaced;
    zval rv;

    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr) ) {
        zval * value = zend_read_property_ex(Z_OBJCE_P(val), val, INTERNED_VALUE, 1, &rv);
        RETURN_ZVAL(value, 1, 0);
    }

    convert_to_string(val);
    replaced = php_escape_html_entities_ex((unsigned char *) Z_STRVAL_P(val), Z_STRLEN_P(val), 0, (int) ENT_QUOTES, "UTF-8", 1);
    RETURN_STR(replaced);
}

PHP_METHOD(HandlebarsUtils, escapeExpression)
{
    zval * val;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_ZVAL(val)
    ZEND_PARSE_PARAMETERS_END();

    php_handlebars_escape_expression(val, return_value);
}
/* }}} Handlebars\Utils::escapeExpression */

/* {{{ proto string Handlebars\Utils::escapeExpressionCompat(mixed value) */
static zend_always_inline char * php_handlebars_escape_expression_replace_helper(char * input)
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

static zend_always_inline void php_handlebars_escape_expression_compat(zval * val, zval * return_value)
{
    zend_string * replaced;
    char * replaced2;
    zval tmp;
    zval rv;

    // @todo this should probably support inheritance
    if( Z_TYPE_P(val) == IS_OBJECT && instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr) ) {
        zval * value = zend_read_property_ex(Z_OBJCE_P(val), val, INTERNED_VALUE, 1, &rv);
        RETURN_ZVAL(value, 1, 0);
    }

    ZVAL_NULL(&tmp);
    if( !php_handlebars_expression(val, &tmp) ) {
        return;
    }

    replaced = php_escape_html_entities_ex((unsigned char *) Z_STRVAL(tmp), Z_STRLEN(tmp), 0, (int) ENT_COMPAT, "UTF-8", 1);
    zval_dtor(&tmp);

    replaced2 = php_handlebars_escape_expression_replace_helper(replaced->val);
    zend_string_free(replaced);

    RETVAL_STRING(replaced2);
    efree(replaced2);
}

PHP_METHOD(HandlebarsUtils, escapeExpressionCompat)
{
    zval * val;

    ZEND_PARSE_PARAMETERS_START(1, 1)
	    Z_PARAM_ZVAL(val)
    ZEND_PARSE_PARAMETERS_END();

    php_handlebars_escape_expression_compat(val, return_value);
}
/* }}} Handlebars\Utils::escapeExpressionCompat */

/* {{{ Argument Info */
PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsUtils, appendContextPath, 2, IS_STRING, 0)
    ZEND_ARG_INFO(0, contextPath)
    ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
PHP_HANDLEBARS_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_createFrame_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsUtils_nameLookup_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, objOrArray)
    ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsUtils, indent, 2, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, indent, IS_STRING, 0)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsUtils, isCallable, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, name)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsUtils, isIntArray, 1, _IS_BOOL, 0)
    ZEND_ARG_ARRAY_INFO(0, arr, 0)
PHP_HANDLEBARS_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsUtils, expression, 1, IS_STRING, 0)
    ZEND_ARG_INFO(0, value)
PHP_HANDLEBARS_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsUtils methods */
static zend_function_entry HandlebarsUtils_methods[] = {
    PHP_ME(HandlebarsUtils, appendContextPath, arginfo_HandlebarsUtils_appendContextPath, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, createFrame, HandlebarsUtils_createFrame_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, nameLookup, HandlebarsUtils_nameLookup_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, indent, arginfo_HandlebarsUtils_indent, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, isCallable, arginfo_HandlebarsUtils_isCallable, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, isIntArray, arginfo_HandlebarsUtils_isIntArray, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, expression, arginfo_HandlebarsUtils_expression, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, escapeExpression, arginfo_HandlebarsUtils_expression, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(HandlebarsUtils, escapeExpressionCompat, arginfo_HandlebarsUtils_expression, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} HandlebarsUtils methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_utils)
{
    zend_class_entry ce;

    INTERNED_CONTEXT_PATH = zend_new_interned_string(zend_string_init(ZEND_STRL("contextPath"), 1));
    INTERNED_VALUE = zend_new_interned_string(zend_string_init(ZEND_STRL("value"), 1));

    // Handlebars\Utils
    INIT_CLASS_ENTRY(ce, "Handlebars\\Utils", HandlebarsUtils_methods);
    HandlebarsUtils_ce_ptr = zend_register_internal_class(&ce);

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
