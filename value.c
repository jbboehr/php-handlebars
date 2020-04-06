
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"

#include <talloc.h>
#include <zend_compile.h>

#include "handlebars.h"
#include "handlebars_private.h"
#include "handlebars_memory.h"
#include "handlebars_value.h"
#include "handlebars_value_handlers.h"

#include "php_handlebars.h"



/* {{{ Variables & Prototypes */
struct handlebars_zval {
    struct handlebars_user usr;
    short callable;
    short int_array;
    zend_fcall_info_cache fcc;
    zval intern;
};
static struct handlebars_value_handlers handlebars_value_std_zval_handlers;
/* }}} Variables & Prototypes */

/* {{{ Utils */
static void handlebars_zval_intern_dtor(struct handlebars_zval * intern) {
    zval_ptr_dtor(&intern->intern);
}
static inline zval * get_intern_zval(struct handlebars_value * value) {
    //struct handlebars_zval * obj = talloc_get_type(value->v.usr, struct handlebars_zval);
    struct handlebars_zval * obj = (struct handlebars_zval *) value->v.usr;
    if( !obj ) {
        return NULL;
    }
    return &obj->intern;
}
static inline void set_intern_zval(struct handlebars_value * value, zval * val) {
    struct handlebars_zval * obj;
    if( !value->v.usr ) {
        obj = talloc_zero(value->ctx, struct handlebars_zval);
        obj->usr.handlers = &handlebars_value_std_zval_handlers;
        value->v.usr = (struct handlebars_user *) obj;
        talloc_set_destructor(obj, handlebars_zval_intern_dtor);
    } else {
        //obj = talloc_get_type(value->v.usr, struct handlebars_zval);
        obj = (struct handlebars_zval *) value->v.usr;
    }
    obj->int_array = -1;
    obj->callable = -1;
    ZVAL_ZVAL(&obj->intern, val, 1, 0);
}
/* }}} Utils */

/* {{{ handlebars zval value handlers */
static struct handlebars_value * handlebars_std_zval_copy(struct handlebars_value * value)
{
    handlebars_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
    return NULL;
}

static void handlebars_std_zval_dtor(struct handlebars_value * value)
{
    //zval * intern = get_intern_zval(value);
    //Z_DELREF_P(intern);
    //zval_ptr_dtor(&intern);
}

static void handlebars_std_zval_convert(struct handlebars_value * value, bool recurse)
{
    handlebars_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
}

static enum handlebars_value_type handlebars_std_zval_type(struct handlebars_value * value)
{
    //struct handlebars_zval * obj = talloc_get_type(value->v.usr, struct handlebars_zval);
    struct handlebars_zval * obj = (struct handlebars_zval *) value->v.usr;
    zval * intern = &obj->intern;

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            if( obj->int_array == -1 ) {
                obj->int_array = php_handlebars_is_int_array(intern);
            }
            if( obj->int_array ) {
                return HANDLEBARS_VALUE_TYPE_ARRAY;
            }
            return HANDLEBARS_VALUE_TYPE_MAP;
        case IS_OBJECT:
            if( obj->callable == -1 ) {
                obj->callable = php_handlebars_is_callable(intern);
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
    zval rval = {0};

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            entry = zend_hash_str_find(Z_ARRVAL_P(intern), key->val, key->len);
            if( !entry ) {
                char * end;
                long index = strtol(key->val, &end, 10);
                if( !*end ) {
                    entry = zend_hash_index_find(Z_ARRVAL_P(intern), index);
                }
            }
            break;
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(intern), zend_ce_arrayaccess) ) {
                zval rv;
                zval prop;
                ZVAL_STRINGL(&prop, key->val, key->len);
                ZVAL_UNDEF(&rv);
#if PHP_MAJOR_VERSION >= 8
                if( Z_OBJ_HT_P(intern)->has_dimension(Z_OBJ_P(intern), &prop, 0) ) {
                    entry = Z_OBJ_HT_P(intern)->read_dimension(Z_OBJ_P(intern), &prop, 0, &rv);
                }
#else
                if( Z_OBJ_HT_P(intern)->has_dimension(intern, &prop, 0) ) {
                    entry = Z_OBJ_HT_P(intern)->read_dimension(intern, &prop, 0, &rv);
                }
#endif
                zval_ptr_dtor(&prop);
                if( entry ) {
                    ret = handlebars_value_from_zval(value->ctx, entry);
                    zval_ptr_dtor(&rv);
                    return ret;
                }
            } else {
                entry = zend_read_property(Z_OBJCE_P(intern), intern, key->val, key->len, 1, NULL);
            }
            if( !entry || Z_TYPE_P(entry) == IS_NULL ) {
                char *error;
                zend_fcall_info_cache fcc;
                zend_function *mptr;
                zval callable;

                array_init(&callable);
                add_next_index_zval(&callable, intern);
                add_next_index_stringl(&callable, key->val, key->len);

                if( zend_is_callable_ex(&callable, NULL, 0, NULL, &fcc, &error) ) {
                    mptr = fcc.function_handler;
                    zend_create_fake_closure(&rval, mptr, mptr->common.scope, fcc.called_scope, intern);
                    entry = &rval;
                }
            }
            break;
    }

    if( entry != NULL ) {
        ret = handlebars_value_from_zval(value->ctx, entry);
    }

    return ret;
}

static struct handlebars_value * handlebars_std_zval_array_find(struct handlebars_value * value, size_t index)
{
    zval * intern = get_intern_zval(value);
    zval * entry = NULL;
    struct handlebars_value * ret = NULL;

    if( Z_TYPE_P(intern) == IS_ARRAY ) {
        entry = zend_hash_index_find(Z_ARRVAL_P(intern), index);
    }

    if( entry != NULL ) {
        ret = handlebars_value_from_zval(value->ctx, entry);
    }

    return ret;
}

static bool handlebars_std_zval_iterator_void(struct handlebars_value_iterator * it)
{
    return false;
}

static bool handlebars_std_zval_iterator_array(struct handlebars_value_iterator * it)
{
    struct handlebars_value * value = it->value;
    zval * intern = get_intern_zval(value);
    HashTable * ht = Z_ARRVAL_P(intern);
    HashPosition * data_pointer = (HashPosition *) it->usr;

    if( it->key ) {
        handlebars_talloc_free(it->key);
        it->key = NULL;
    }

    if( it->current != NULL ) {
        handlebars_value_delref(it->current);
        it->current = NULL;
    }

    zval * entry = zend_hash_get_current_data_ex(ht, data_pointer);
    zend_string *string_key;
    zend_ulong num_key;
    if( entry ) {
        if( HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(ht, &string_key, &num_key, data_pointer) ) {
            it->key = handlebars_string_ctor(value->ctx, ZSTR_VAL(string_key), ZSTR_LEN(string_key));
            it->index = 0;
        } else {
            it->key = NULL;
            it->index = num_key;
        }
        it->current = handlebars_value_from_zval(value->ctx, entry);
        handlebars_value_addref(it->current);
        zend_hash_move_forward_ex(ht, data_pointer);
        return true;
    }

    return false;
}

bool handlebars_std_zval_iterator_init(struct handlebars_value_iterator * it, struct handlebars_value * value)
{
    zval * intern = get_intern_zval(value);
    HashPosition * data_pointer = handlebars_talloc_zero(value->ctx, HashPosition);
    HashTable * ht;

    it->value = value;

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            ht = HASH_OF(intern);
            data_pointer = handlebars_talloc_zero(value->ctx, HashPosition);
            it->usr = (void *) data_pointer;
            it->length = zend_hash_num_elements(ht);
            zval * entry;
            zend_string *string_key;
            zend_ulong num_key;

            zend_hash_internal_pointer_reset_ex(ht, data_pointer);
            entry = zend_hash_get_current_data_ex(ht, data_pointer);
            if( entry ) {
                if( HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(ht, &string_key, &num_key, data_pointer) ) {
                    it->key = handlebars_string_ctor(value->ctx, ZSTR_VAL(string_key), ZSTR_LEN(string_key));
                    it->index = 0;
                } else {
                    it->key = NULL;
                    it->index = num_key;
                }
                it->current = handlebars_value_from_zval(value->ctx, entry);
                it->next = &handlebars_std_zval_iterator_array;
                handlebars_value_addref(it->current);
                zend_hash_move_forward_ex(ht, data_pointer);
                return true;
            }
            break;
        case IS_OBJECT:
            // @todo
            break;
        default:
            // Do nothing
            break;
    }

    it->next = &handlebars_std_zval_iterator_void;

    return false;
}

long handlebars_std_zval_count(struct handlebars_value * value)
{
    zval * intern = get_intern_zval(value);

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            return zend_hash_num_elements(Z_ARRVAL_P(intern));
        case IS_OBJECT: // @todo
        default:
            return -1;
    }

}

struct handlebars_value * handlebars_std_zval_call(struct handlebars_value * value, HANDLEBARS_HELPER_ARGS)
{
    //struct handlebars_zval * obj = talloc_get_type(value->v.usr, struct handlebars_zval);
    struct handlebars_zval * obj = (struct handlebars_zval *) value->v.usr;
    zval * intern = get_intern_zval(value);
    zval * z_ret;
    zend_function *fptr;
    zend_bool is_closure = 0;
	zend_class_entry *ce = NULL;
	uint32_t num_args;
	struct _zend_arg_info *arg_info;
    short send_options = 0;

    // Check if is callable object (closure or __invoke)
    if( !intern || Z_TYPE_P(intern) != IS_OBJECT ) {
        return NULL;
    }

    if( obj->callable == -1 ) {
        obj->callable = php_handlebars_is_callable(intern);
    }

    if( !obj->callable ) {
        return NULL;
    }

    // If the argument is a closure: we will omit passing the options
    // object if it will go into a slot with an incompatible type
    // If the argument is a callable object: we will append the
    // options if the appended parameter will go into a
    // declared parameter with either no type signature or explicitly
    // typed with Handlebars\Options
    ce = Z_OBJCE_P(intern);
    if (instanceof_function(ce, zend_ce_closure)) {
        fptr = (zend_function *)zend_get_closure_method_def(intern);
        send_options = 1;
    } else {
        fptr = zend_hash_find_ptr(&ce->function_table, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE));
    }
    if (fptr) {
        arg_info = fptr->common.arg_info;
        num_args = fptr->common.num_args;

        if (argc < num_args) {
            if (ZEND_TYPE_IS_CLASS((arg_info + argc)->type)) {
                if (0 == strcmp(ZSTR_VAL(ZEND_TYPE_NAME((arg_info + argc)->type)), "Handlebars\\Options")) {
                    send_options = 1;
                } else {
                    send_options = 0;
                }
            } else if (ZEND_TYPE_IS_CODE(arg_info->type)) {
                send_options = 0;
            } else {
                send_options = 1;
            }
        }
    }

    // Convert paramsma
    int n_args = argc + (send_options ? 1 : 0);
    zval *z_args = alloca(sizeof(zval) * n_args);
    memset(z_args, 0, sizeof(z_args));

    int i;
    for( i = 0; i < argc; i++ ) {
        handlebars_value_to_zval(argv[i], &z_args[i]);
    }

    if (send_options) {
        zval z_options;
        php_handlebars_options_ctor(options, &z_options);
        z_args[n_args - 1] = z_options;
    }

    // Make ret
    zval _z_ret;
    z_ret = &_z_ret;

    zend_fcall_info fci = {0};
    fci.size = sizeof(fci);
    fci.object = Z_OBJ_P(intern);
    fci.retval = &_z_ret;
    fci.params = z_args;
    fci.param_count = n_args;
    fci.no_separation = 1;

    ZVAL_STRING(&fci.function_name, "__invoke");

    if( zend_call_function(&fci, &obj->fcc) == FAILURE ) {
#if PHP_MAJOR_VERSION >= 8
        zend_throw_exception_ex(zend_ce_exception, 0, "Could not execute %s::%s()", ZSTR_VAL(Z_OBJCE_P(intern)->name), ZSTR_VAL(Z_OBJCE_P(intern)->constructor->common.function_name));
#else
        zend_throw_exception_ex(zend_ce_exception, 0, "Could not execute %s::%s()", Z_OBJCE_P(intern)->name, Z_OBJCE_P(intern)->constructor->common.function_name);
#endif
    }

    for( i = 0; i < n_args; i++ ) {
        zval_ptr_dtor(&z_args[i]);
    }
    zval_ptr_dtor(&fci.function_name);

    struct handlebars_value * retval = NULL;

    if( Z_TYPE_P(z_ret) == IS_OBJECT && instanceof_function(Z_OBJCE_P(z_ret), HandlebarsSafeString_ce_ptr) ) {
        convert_to_string(z_ret);
        retval = handlebars_value_from_zval(HBSCTX(options->vm), z_ret);
        retval->flags |= HANDLEBARS_VALUE_FLAG_SAFE_STRING;
    } else {
        retval = handlebars_value_from_zval(HBSCTX(options->vm), z_ret);
    }

    zval_ptr_dtor(z_ret);

    if( EG(exception) ) {
        handlebars_throw(HBSCTX(options->vm), HANDELBARS_EXTERNAL_ERROR, "external error");
    }

    return retval;
}

static struct handlebars_value_handlers handlebars_value_std_zval_handlers = {
        "zval",
        &handlebars_std_zval_copy,
        &handlebars_std_zval_dtor,
        &handlebars_std_zval_convert,
        &handlebars_std_zval_type,
        &handlebars_std_zval_map_find,
        &handlebars_std_zval_array_find,
        &handlebars_std_zval_iterator_init,
        &handlebars_std_zval_call,
        &handlebars_std_zval_count
};
/* }}} handlebars zval value handlers */

/* {{{ handlebars_value_to_zval */
static inline void handlebars_value_array_to_zval(struct handlebars_value * value, zval * val)
{
    struct handlebars_value_iterator it;
    handlebars_value_iterator_init(&it, value);
    array_init(val);

    for( ; it.current != NULL; it.next(&it) ) {
        zval tmp = {0};
        handlebars_value_to_zval(it.current, &tmp);
        add_next_index_zval(val, &tmp);
    }
}

static inline void handlebars_value_map_to_zval(struct handlebars_value * value, zval * val)
{
    struct handlebars_value_iterator it;
    handlebars_value_iterator_init(&it, value);
    array_init(val);

    for( ; it.current != NULL; it.next(&it) ) {
        zval tmp = {0};
        handlebars_value_to_zval(it.current, &tmp);
        add_assoc_zval_ex(val, it.key->val, it.key->len, &tmp); // @todo does this need + 1
    }
}

PHP_HANDLEBARS_API zval * handlebars_value_to_zval(struct handlebars_value * value, zval * val)
{
    zval * intern;

    switch( value->type ) {
        case HANDLEBARS_VALUE_TYPE_NULL:
            ZVAL_NULL(val);
            break;
        case HANDLEBARS_VALUE_TYPE_TRUE:
            ZVAL_BOOL(val, 1);
            break;
        case HANDLEBARS_VALUE_TYPE_FALSE:
            ZVAL_BOOL(val, 0);
            break;
        case HANDLEBARS_VALUE_TYPE_FLOAT:
            ZVAL_DOUBLE(val, value->v.dval);
            break;
        case HANDLEBARS_VALUE_TYPE_INTEGER:
            ZVAL_LONG(val, value->v.lval);
            break;
        case HANDLEBARS_VALUE_TYPE_STRING:
            ZVAL_STRINGL(val, handlebars_value_get_strval(value), handlebars_value_get_strlen(value));
            break;
        case HANDLEBARS_VALUE_TYPE_ARRAY:
            handlebars_value_array_to_zval(value, val);
            break;
        case HANDLEBARS_VALUE_TYPE_MAP:
            handlebars_value_map_to_zval(value, val);
            break;
        case HANDLEBARS_VALUE_TYPE_USER:
            intern = get_intern_zval(value);
            // @todo check to make sure it's a zval type
            ZVAL_ZVAL(val, intern, 1, 0);
            break;

        default: // This shouldn't happen
            ZVAL_NULL(val);
            break;
    }

    return val;
}
/* }}} handlebars_value_to_zval */

/* {{{ handlebars_value_from_zval */
PHP_HANDLEBARS_API struct handlebars_value * handlebars_value_from_zval(struct handlebars_context * context, zval * val)
{
    struct handlebars_value * value = handlebars_value_ctor(context);

    switch( Z_TYPE_P(val) ) {
        case IS_UNDEF:
        case IS_NULL:
            // do nothing
            break;
        case IS_TRUE:
            value->type = HANDLEBARS_VALUE_TYPE_TRUE;
            break;
        case IS_FALSE:
            value->type = HANDLEBARS_VALUE_TYPE_FALSE;
            break;
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
