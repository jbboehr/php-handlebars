
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"

#include "handlebars.h"
#include "handlebars_private.h"
#include "handlebars_memory.h"
#include "handlebars_value.h"
#include "handlebars_value_handlers.h"

#include "php5to7.h"
#include "php_handlebars.h"



/* {{{ Variables & Prototypes */
struct handlebars_zval {
    short callable;
    short int_array;
    zend_fcall_info_cache fcc;
#ifdef ZEND_ENGINE_3
    zval intern;
#else
    zval * intern;
#endif
};
/* }}} Variables & Prototypes */

/* {{{ Utils */
static handlebars_zval_intern_dtor(struct handlebars_zval * intern) {
#ifdef ZEND_ENGINE_3
    zval_ptr_dtor(&intern->intern);
#else
    if( intern->intern ) {
        zval_ptr_dtor(&intern->intern);
    }
#endif
}
static inline zval * get_intern_zval(struct handlebars_value * value) {
    struct handlebars_zval * obj = talloc_get_type(value->v.usr, struct handlebars_zval);
    if( !obj ) {
        return NULL;
    }
#ifdef ZEND_ENGINE_3
    return &obj->intern;
#else
    return obj->intern;
#endif
}
static inline void set_intern_zval(struct handlebars_value * value, zval * val) {
    struct handlebars_zval * obj;
    if( !value->v.usr ) {
        value->v.usr = obj = talloc_zero(value->ctx, struct handlebars_zval);
#ifndef ZEND_ENGINE_3
        MAKE_STD_ZVAL(obj->intern);
#endif
        talloc_set_destructor(obj, handlebars_zval_intern_dtor);
    } else {
        obj = talloc_get_type(value->v.usr, struct handlebars_zval);
    }
    obj->int_array = -1;
    obj->callable = -1;
#ifdef ZEND_ENGINE_3
    ZVAL_ZVAL(&obj->intern, val, 1, 0);
#else
    ZVAL_ZVAL(obj->intern, val, 1, 0);
#endif
}
/* }}} Utils */

/* {{{ handlebars zval value handlers */
static struct handlebars_value * handlebars_std_zval_copy(struct handlebars_value * value)
{
    handlebars_context_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
    return NULL;
}

static void handlebars_std_zval_dtor(struct handlebars_value * value)
{
    zval * intern = get_intern_zval(value);
    //Z_DELREF_P(intern);
    //zval_ptr_dtor(&intern);
}

static void handlebars_std_zval_convert(struct handlebars_value * value, bool recurse)
{
    handlebars_context_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
}

static enum handlebars_value_type handlebars_std_zval_type(struct handlebars_value * value)
{
    struct handlebars_zval * obj = talloc_get_type(value->v.usr, struct handlebars_zval);
    zval * intern = get_intern_zval(value);
    TSRMLS_FETCH();

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            if( obj->int_array == -1 ) {
                obj->int_array = php_handlebars_is_int_array(intern TSRMLS_CC);
            }
            if( obj->int_array ) {
                return HANDLEBARS_VALUE_TYPE_ARRAY;
            }
            return HANDLEBARS_VALUE_TYPE_MAP;
        case IS_OBJECT:
            if( obj->callable == -1 ) {
                obj->callable = php_handlebars_is_callable(intern TSRMLS_CC);
            }
            if( obj->callable ) {
                return HANDLEBARS_VALUE_TYPE_HELPER;
            }
            return HANDLEBARS_VALUE_TYPE_MAP;
        default:
            assert(0);
            break;
    }
    return HANDLEBARS_VALUE_TYPE_NULL;
}

static struct handlebars_value * handlebars_std_zval_map_find(struct handlebars_value * value, struct handlebars_string * key)
{
    zval * intern = get_intern_zval(value);
    zval * entry = NULL;
    struct handlebars_value * ret = NULL;
    TSRMLS_FETCH();

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            entry = php5to7_zend_hash_find(Z_ARRVAL_P(intern), key->val, key->len);
            if( !entry ) {
                char * end;
                long index = strtod(key->val, &end);
                if( !*end ) {
                    entry = php5to7_zend_hash_index_find(Z_ARRVAL_P(intern), index);
                }
            }
            break;
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(intern), zend_ce_arrayaccess TSRMLS_CC) ) {
#ifdef ZEND_ENGINE_3
                zval prop;
                ZVAL_STRINGL(&prop, key->val, key->len);
                if( Z_OBJ_HT_P(intern)->has_dimension(intern, &prop, 0 TSRMLS_CC) ) {
                    entry = Z_OBJ_HT_P(intern)->read_dimension(intern, &prop, 0, entry TSRMLS_CC);
                }
                zval_ptr_dtor(&prop);
#else
                zval * prop;
                MAKE_STD_ZVAL(prop);
                ZVAL_STRINGL(prop, key->val, key->len, 1);
                if( Z_OBJ_HT_P(intern)->has_dimension(intern, prop, 0 TSRMLS_CC) ) {
                    entry = Z_OBJ_HT_P(intern)->read_dimension(intern, prop, 0 TSRMLS_CC);
                }
                zval_ptr_dtor(&prop);
#endif
            } else {
                entry = php5to7_zend_read_property2(Z_OBJCE_P(intern), intern, key->val, key->len, 1);
            }
            break;
    }

    if( entry != NULL ) {
        ret = handlebars_value_from_zval(value->ctx, entry TSRMLS_CC);
    }

    return ret;
}

static struct handlebars_value * handlebars_std_zval_array_find(struct handlebars_value * value, size_t index)
{
    zval * intern = get_intern_zval(value);
    zval * entry = NULL;
    struct handlebars_value * ret = NULL;
    TSRMLS_FETCH();

    if( Z_TYPE_P(intern) == IS_ARRAY ) {
        entry = php5to7_zend_hash_index_find(Z_ARRVAL_P(intern), index);
    }

    if( entry != NULL ) {
        ret = handlebars_value_from_zval(value->ctx, entry TSRMLS_CC);
    }

    return ret;
}

struct handlebars_value_iterator * handlebars_std_zval_iterator_ctor(struct handlebars_value * value)
{
    zval * intern = get_intern_zval(value);
    struct handlebars_value_iterator * it = handlebars_talloc_zero(value, struct handlebars_value_iterator);
    zval ** data_entry = NULL;
    HashPosition * data_pointer = handlebars_talloc_zero(value->ctx, HashPosition);
    HashTable * ht;
    TSRMLS_FETCH();

    it->value = value;

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            ht = HASH_OF(intern);
            data_pointer = handlebars_talloc_zero(value->ctx, HashPosition);
            it->usr = (void *) data_pointer;
            it->length = zend_hash_num_elements(ht);
            do {
#ifdef ZEND_ENGINE_3
                zval * entry;
                zend_string *string_key;
                zend_ulong num_key;

                zend_hash_internal_pointer_reset_ex(ht, data_pointer);
                entry = zend_hash_get_current_data_ex(ht, data_pointer);
                if( entry ) {
                    if( HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(ht, &string_key, &num_key, data_pointer) ) {
                        it->key = handlebars_talloc_strndup(it, ZSTR_VAL(string_key), ZSTR_LEN(string_key));
                        it->index = 0;
                    } else {
                        it->key = NULL;
                        it->index = num_key;
                    }
                    it->current = handlebars_value_from_zval(value->ctx, entry);
                    handlebars_value_addref(it->current);
                    zend_hash_move_forward_ex(ht, data_pointer);
                }
#else
                int key_type = 0;
                char * key_str = NULL;
                uint key_len = 0;
                ulong key_nindex = 0;
                zend_hash_internal_pointer_reset_ex(ht, data_pointer);
                if( SUCCESS == zend_hash_get_current_data_ex(ht, (void**) &data_entry, data_pointer) ) {
                    key_type = zend_hash_get_current_key_ex(ht, &key_str, &key_len, &key_nindex, false, data_pointer);
                    if( key_type == HASH_KEY_IS_STRING ) {
                        it->key = key_str;
                        it->index = 0;
                    } else {
                        it->key = NULL;
                        it->index = key_nindex;
                    }
                    it->current = handlebars_value_from_zval(value->ctx, *data_entry TSRMLS_CC);
                    handlebars_value_addref(it->current);
                    zend_hash_move_forward_ex(ht, data_pointer);
                }
#endif
            } while(0);
            break;
        case IS_OBJECT:
            // @todo
            break;
        default:
            handlebars_talloc_free(it);
            it = NULL;
            break;
    }

    return it;
}

bool handlebars_std_zval_iterator_next(struct handlebars_value_iterator * it)
{
    bool ret = false;
    struct handlebars_value * value = it->value;
    zval * intern = get_intern_zval(value);
    HashTable * ht;
    HashPosition * data_pointer;
    TSRMLS_FETCH();

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            ht = Z_ARRVAL_P(intern);
            data_pointer = (HashPosition *) it->usr;
            do {
#ifdef ZEND_ENGINE_3
                zval * entry = zend_hash_get_current_data_ex(ht, data_pointer);
                zend_string *string_key;
                zend_ulong num_key;
                if( entry ) {
                    ret = true;
                    if( HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(ht, &string_key, &num_key, data_pointer) ) {
                        it->key = handlebars_talloc_strndup(it, ZSTR_VAL(string_key), ZSTR_LEN(string_key));
                        it->index = 0;
                    } else {
                        it->key = NULL;
                        it->index = num_key;
                    }
                    it->current = handlebars_value_from_zval(value->ctx, entry);
                    handlebars_value_addref(it->current);
                    zend_hash_move_forward_ex(ht, data_pointer);
                }
#else
                zval ** data_entry = NULL;
                int key_type = 0;
                char * key_str = NULL;
                uint key_len = 0;
                ulong key_nindex = 0;
                if( SUCCESS == zend_hash_get_current_data_ex(ht, (void**) &data_entry, data_pointer) ) {
                    key_type = zend_hash_get_current_key_ex(ht, &key_str, &key_len, &key_nindex, false, data_pointer);

                    ret = true;
                    if( key_type == HASH_KEY_IS_STRING ) {
                        it->key = key_str;
                        it->index = 0;
                    } else {
                        it->key = NULL;
                        it->index = key_nindex;
                    }
                    it->current = handlebars_value_from_zval(value->ctx, *data_entry TSRMLS_CC);
                    handlebars_value_addref(it->current);
                    zend_hash_move_forward_ex(ht, data_pointer);
                }
#endif
            } while(0);
            break;
        case IS_OBJECT:
            break;
        default:
            handlebars_talloc_free(it);
            it = NULL;
            break;

    }

    return ret;
}

long handlebars_std_zval_count(struct handlebars_value * value)
{
    zval * intern = get_intern_zval(value);

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            return zend_hash_num_elements(Z_ARRVAL_P(intern));
        case IS_OBJECT:
            return -1; // @todo
    }

    return -1;

}

struct handlebars_value * handlebars_std_zval_call(struct handlebars_value * value, struct handlebars_options * options)
{
    struct handlebars_zval * obj = value->v.ptr;
    zval * intern = get_intern_zval(value);
    zval * z_ret;
    TSRMLS_FETCH();

    // Check if is callable object (closure or __invoke)
    if( !intern || Z_TYPE_P(intern) != IS_OBJECT ) {
        return NULL;
    }

    if( obj->callable == -1 ) {
        obj->callable = php_handlebars_is_callable(intern TSRMLS_CC);
    }

    if( !obj->callable ) {
        return NULL;
    }

#ifdef ZEND_ENGINE_3
    zval z_options;
    php_handlebars_options_ctor(options, &z_options TSRMLS_CC);

    // Convert params
    size_t n_args = handlebars_stack_length(options->params) + 1;
    zval *z_const_args = emalloc(n_args * sizeof(zval));

    int i;
    for( i = 0; i < n_args - 1; i++ ) {
        struct handlebars_value * val = handlebars_stack_get(options->params, i);
        handlebars_value_to_zval(val, &z_const_args[i]);
    }

    z_const_args[n_args - 1] = z_options;

    // Make ret
    zval _z_ret;
    z_ret = &_z_ret;

    zend_fcall_info fci;
    fci.size = sizeof(fci);
    fci.function_table = &Z_OBJCE_P(intern)->function_table;
    fci.symbol_table = NULL;
    fci.object = Z_OBJ_P(intern);
    fci.retval = &_z_ret;
    fci.params = z_const_args;
    fci.param_count = n_args;
    fci.no_separation = 1;

    ZVAL_STRING(&fci.function_name, "__invoke");

    if( zend_call_function(&fci, &obj->fcc) == FAILURE ) {
        zend_throw_exception_ex(zend_ce_exception, 0, "Could not execute %s::%s()", Z_OBJCE_P(intern)->name, Z_OBJCE_P(intern)->constructor->common.function_name);
    }

    for( i = 0; i < n_args; i++ ) {
        zval_ptr_dtor(&z_const_args[i]);
    }
    zval_ptr_dtor(&fci.function_name);
    efree(z_const_args);

#else
    // Construct options
    zval * z_options;
    MAKE_STD_ZVAL(z_options);
    php_handlebars_options_ctor(options, z_options TSRMLS_CC);

    // Convert params
    size_t n_args = handlebars_stack_length(options->params) + 1;
    zval **z_const_args = emalloc(n_args * sizeof(zval *));

    int i;
    for( i = 0; i < n_args - 1; i++ ) {
        struct handlebars_value * val = handlebars_stack_get(options->params, i);
        MAKE_STD_ZVAL(z_const_args[i]);
        handlebars_value_to_zval(val, z_const_args[i] TSRMLS_CC);
    }

    z_const_args[n_args - 1] = z_options;

    // Call
    zval * z_const;
    MAKE_STD_ZVAL(z_const);
    MAKE_STD_ZVAL(z_ret);
    ZVAL_STRING(z_const, "__invoke", 1);
    call_user_function(&Z_OBJCE_P(intern)->function_table, &intern, z_const, z_ret, n_args, z_const_args TSRMLS_CC);
    for( i = 0; i < n_args; i++ ) {
        zval_ptr_dtor(&z_const_args[i]);
    }
    efree(z_const_args);
    zval_ptr_dtor(&z_const);
#endif

    struct handlebars_value * retval = NULL;
    bool is_safe_string = false;

    switch( Z_TYPE_P(z_ret) ) {
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(z_ret), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
                convert_to_string(z_ret);
                retval = handlebars_value_from_zval(HBSCTX(options->vm), z_ret TSRMLS_CC);
                retval->flags |= HANDLEBARS_VALUE_FLAG_SAFE_STRING;
                break;
            }
            // fall-through
        case IS_ARRAY:
        case IS_NULL:
#ifdef ZEND_ENGINE_3
            case IS_TRUE:
        case IS_FALSE:
#else
        case IS_BOOL:
#endif
        case IS_LONG:
        case IS_DOUBLE:
        case IS_STRING:
            retval = handlebars_value_from_zval(HBSCTX(options->vm), z_ret TSRMLS_CC);
            break;
        default:
            break;
    }

#ifdef ZEND_ENGINE_3
    zval_ptr_dtor(z_ret);
#else
    // @todo this seems to cause a problem
    zval_ptr_dtor(&z_ret);
#endif

    return retval;
}

static struct handlebars_value_handlers handlebars_value_std_zval_handlers = {
        &handlebars_std_zval_copy,
        &handlebars_std_zval_dtor,
        &handlebars_std_zval_convert,
        &handlebars_std_zval_type,
        &handlebars_std_zval_map_find,
        &handlebars_std_zval_array_find,
        &handlebars_std_zval_iterator_ctor,
        &handlebars_std_zval_iterator_next,
        &handlebars_std_zval_call,
        &handlebars_std_zval_count
};
/* }}} handlebars zval value handlers */

/* {{{ handlebars_value_to_zval */
static inline handlebars_value_array_to_zval(struct handlebars_value * value, zval * val TSRMLS_DC)
{
    struct handlebars_value_iterator * it = handlebars_value_iterator_ctor(value);
    array_init(val);

    for( ; it->current != NULL; handlebars_value_iterator_next(it) ) {
#ifdef ZEND_ENGINE_3
        zval _tmp;
        zval * tmp = &_tmp;
#else
        zval * tmp;
        MAKE_STD_ZVAL(tmp);
#endif
        handlebars_value_to_zval(it->current, tmp TSRMLS_CC);
        add_next_index_zval(val, tmp);
    }
}

static inline handlebars_value_map_to_zval(struct handlebars_value * value, zval * val TSRMLS_DC)
{
    struct handlebars_value_iterator * it = handlebars_value_iterator_ctor(value);
    array_init(val);

    for( ; it->current != NULL; handlebars_value_iterator_next(it) ) {
#ifdef ZEND_ENGINE_3
        zval _tmp;
        zval * tmp = &_tmp;
        handlebars_value_to_zval(it->current, tmp);
        add_assoc_zval_ex(val, it->key, strlen(it->key), tmp); // @todo does this need + 1
#else
        zval * tmp;
        MAKE_STD_ZVAL(tmp);
        handlebars_value_to_zval(it->current, tmp TSRMLS_CC);
        add_assoc_zval_ex(val, it->key, strlen(it->key) + 1, tmp);
#endif
    }
}

PHPAPI zval * handlebars_value_to_zval(struct handlebars_value * value, zval * val TSRMLS_DC)
{
    zval * intern;

    switch( value->type ) {
        case HANDLEBARS_VALUE_TYPE_NULL:
        ZVAL_NULL(val);
            break;
        case HANDLEBARS_VALUE_TYPE_BOOLEAN:
            ZVAL_BOOL(val, value->v.bval);
            break;
        case HANDLEBARS_VALUE_TYPE_FLOAT:
        ZVAL_DOUBLE(val, value->v.dval);
            break;
        case HANDLEBARS_VALUE_TYPE_INTEGER:
        ZVAL_LONG(val, value->v.lval);
            break;
        case HANDLEBARS_VALUE_TYPE_STRING:
            PHP5TO7_ZVAL_STRINGL(val, handlebars_value_get_strval(value), handlebars_value_get_strlen(value));
            break;
        case HANDLEBARS_VALUE_TYPE_ARRAY:
            handlebars_value_array_to_zval(value, val TSRMLS_CC);
            break;
        case HANDLEBARS_VALUE_TYPE_MAP:
            handlebars_value_map_to_zval(value, val TSRMLS_CC);
            break;
        case HANDLEBARS_VALUE_TYPE_USER:
            intern = get_intern_zval(value);
            // @todo check to make sure it's a zval type
            ZVAL_ZVAL(val, intern, 1, 0);
            break;
    }

    return val;
}
/* }}} handlebars_value_to_zval */

/* {{{ handlebars_value_from_zval */
PHPAPI struct handlebars_value * handlebars_value_from_zval(struct handlebars_context * context, zval * val TSRMLS_DC)
{
    struct handlebars_value * value = handlebars_value_ctor(context);

    switch( Z_TYPE_P(val) ) {
#ifdef ZEND_ENGINE_3
        case IS_UNDEF:
#endif
        case IS_NULL:
            // do nothing
            break;
#ifdef ZEND_ENGINE_3
        case IS_TRUE:
            value->type = HANDLEBARS_VALUE_TYPE_BOOLEAN;
            value->v.bval = true;
            break;
        case IS_FALSE:
            value->type = HANDLEBARS_VALUE_TYPE_BOOLEAN;
            value->v.bval = false;
            break;
#else
        case IS_BOOL:
            value->type = HANDLEBARS_VALUE_TYPE_BOOLEAN;
            value->v.bval = Z_BVAL_P(val);
            break;
#endif
        case IS_DOUBLE:
            value->type = HANDLEBARS_VALUE_TYPE_FLOAT;
            value->v.dval = Z_DVAL_P(val);
            break;
        case IS_LONG:
            value->type = HANDLEBARS_VALUE_TYPE_INTEGER;
            // @todo make sure sizing is correct
            value->v.lval = Z_LVAL_P(val);
            break;
        case IS_STRING:
            value->type = HANDLEBARS_VALUE_TYPE_STRING;
            handlebars_value_stringl(value, Z_STRVAL_P(val), Z_STRLEN_P(val));
            break;

        case IS_OBJECT:
            // fall-through
        case IS_ARRAY:
            value->type = HANDLEBARS_VALUE_TYPE_USER;
            value->handlers = &handlebars_value_std_zval_handlers;
            set_intern_zval(value, val);
            break;
        default:
            // ruh roh
            assert(0);
            break;
    }

    return value;
}
/* }}} handlebars_value_from_zval */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_value)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;

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
