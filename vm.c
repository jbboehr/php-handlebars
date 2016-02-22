
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"

#include "handlebars_private.h"

#include "handlebars.h"
#include "handlebars_cache.h"
#include "handlebars_compiler.h"
#include "handlebars_helpers.h"
#include "handlebars_memory.h"
#include "handlebars_opcodes.h"
#include "handlebars_string.h"
#include "handlebars_value.h"
#include "handlebars_value_handlers.h"
#include "handlebars_vm.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsVM_ce_ptr;
static zend_object_handlers HandlebarsVM_obj_handlers;

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

struct php_handlebars_vm_obj {
#if PHP_MAJOR_VERSION < 7
    zend_object std;
#endif
    //struct handlebars_vm * vm;
    TALLOC_CTX * mctx;
    struct handlebars_context * context;
    struct handlebars_vm * vm;
    struct handlebars_value * helpers;
    struct handlebars_value * partials;
#if PHP_MAJOR_VERSION >= 7
    zend_object std;
#endif
};
/* }}} Variables & Prototypes */

/* {{{ Z_HANDLEBARS_VM_P */
#ifdef ZEND_ENGINE_3
static inline struct php_handlebars_vm_obj * php_handlebars_vm_fetch_object(zend_object *obj) {
    return (struct php_handlebars_vm_obj *)((char*)(obj) - XtOffsetOf(struct php_handlebars_vm_obj, std));
}
#define Z_HANDLEBARS_VM_P(zv) php_handlebars_vm_fetch_object(Z_OBJ_P((zv)))
#else
#define Z_HANDLEBARS_VM_P(zv) zend_object_store_get_object(zv TSRMLS_CC)
#endif
/* }}} */

/* {{{ php_handlebars_vm_obj_free */
#ifdef ZEND_ENGINE_3
static void php_handlebars_vm_obj_free(zend_object * object TSRMLS_DC)
{
    struct php_handlebars_vm_obj * obj = php_handlebars_vm_fetch_object(object TSRMLS_CC);

    if( obj->helpers ) {
        handlebars_value_dtor(obj->helpers);
    }
    if( obj->partials ) {
        handlebars_value_dtor(obj->partials);
    }
    handlebars_context_dtor(obj->context);

    zend_object_std_dtor((zend_object *)object TSRMLS_CC);
}
#else
static void php_handlebars_vm_obj_free(void *object TSRMLS_DC)
{
    struct php_handlebars_vm_obj * obj = (struct php_handlebars_vm_obj *) object;

    if( obj->helpers ) {
        handlebars_value_dtor(obj->helpers);
    }
    if( obj->partials    ) {
        handlebars_value_dtor(obj->partials);
    }
    handlebars_context_dtor(obj->context);
    if( obj->mctx ) {
        handlebars_talloc_free(obj->mctx);
    }

    zend_object_std_dtor(&obj->std TSRMLS_CC);
    efree(object);
}
#endif
/* }}} */

/* {{{ php_handlebars_vm_obj_create */
static inline void php_handlebars_vm_obj_create_common(struct php_handlebars_vm_obj *obj)
{
    zend_long pool_size = HANDLEBARS_G(pool_size);

    if( pool_size > 0 ) {
        obj->mctx = talloc_pool(HANDLEBARS_G(root), pool_size);
        obj->context = handlebars_context_ctor_ex(obj->mctx);
    } else {
        obj->context = handlebars_context_ctor_ex(HANDLEBARS_G(root));
    }
    obj->vm = handlebars_vm_ctor(obj->context);
    obj->vm->cache = HANDLEBARS_G(cache);
    obj->vm->helpers = obj->helpers = handlebars_value_ctor(obj->context);
    handlebars_value_map_init(obj->helpers);
    obj->vm->partials = obj->partials = handlebars_value_ctor(obj->context);
    handlebars_value_map_init(obj->partials);
}
#ifdef ZEND_ENGINE_3
static zend_object * php_handlebars_vm_obj_create(zend_class_entry * ce)
{
    struct php_handlebars_vm_obj *obj;

    obj = ecalloc(1, sizeof(*obj) + zend_object_properties_size(ce));
    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);
    obj->std.handlers = &HandlebarsVM_obj_handlers;
    php_handlebars_vm_obj_create_common(obj);

    return &obj->std;
}
#else
zend_object_value php_handlebars_vm_obj_create(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    struct php_handlebars_vm_obj *obj;
    zval *tmp;

    obj = ecalloc(1, sizeof(struct php_handlebars_vm_obj));
    zend_object_std_init(&obj->std, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_property_ctor, (void *) &tmp, sizeof(zval *));
#else
    object_properties_init(&obj->std, ce);
#endif

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)php_handlebars_vm_obj_free, NULL TSRMLS_CC);
    retval.handlers = &HandlebarsVM_obj_handlers;
    php_handlebars_vm_obj_create_common(obj);

    return retval;
}
#endif
/* }}} */


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

    // Add scope
    if( options->scope ) {
        zval z_scope;
        handlebars_value_to_zval(options->scope, &z_scope);
        zend_update_property(Z_OBJCE(z_options), &z_options, ZEND_STRL("scope"), &z_scope);
        zval_ptr_dtor(&z_scope);
    }

    // Add hash
    if( options->hash ) {
        zval z_hash;
        handlebars_value_to_zval(options->hash, &z_hash);
        zend_update_property(Z_OBJCE(z_options), &z_options, ZEND_STRL("hash"), &z_hash);
        zval_ptr_dtor(&z_hash);
    }

    // Add data
    if( options->data ) {
        zval z_data;
        handlebars_value_to_zval(options->data, &z_data);
        zend_update_property(Z_OBJCE(z_options), &z_options, ZEND_STRL("data"), &z_data TSRMLS_CC);
        zval_ptr_dtor(&z_data);
    }

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

    // Add scope
    if( options->scope ) {
        zval *z_scope;
        MAKE_STD_ZVAL(z_scope);
        handlebars_value_to_zval(options->scope, z_scope TSRMLS_CC);
        zend_update_property(Z_OBJCE_P(z_options), z_options, ZEND_STRL("scope"), z_scope TSRMLS_CC);
        zval_ptr_dtor(&z_scope);
    }

    // Add hash
    if( options->hash ) {
        zval *z_hash;
        MAKE_STD_ZVAL(z_hash);
        handlebars_value_to_zval(options->hash, z_hash TSRMLS_CC);
        zend_update_property(Z_OBJCE_P(z_options), z_options, ZEND_STRL("hash"), z_hash TSRMLS_CC);
        zval_ptr_dtor(&z_hash);
    }

    // Add data
    if( options->data ) {
        zval *z_data;
        MAKE_STD_ZVAL(z_data);
        handlebars_value_to_zval(options->data, z_data TSRMLS_CC);
        zend_update_property(Z_OBJCE_P(z_options), z_options, ZEND_STRL("data"), z_data TSRMLS_CC);
        zval_ptr_dtor(&z_data);
    }

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

/* {{{ handlebars_value_array_to_zval */
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
/* }}} handlebars_value_array_to_zval */

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

/* {{{ php_handlebars_fetch_known_helpers */
void php_handlebars_fetch_known_helpers(struct handlebars_compiler * compiler, zval * helpers TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    HashPosition data_pointer = NULL;
    zval ** data_entry = NULL;
    long num;
    long idx = 0;
    char ** known_helpers;

    if( Z_TYPE_P(helpers) == IS_ARRAY ) {
        data_hash = Z_ARRVAL_P(helpers);
    } else if( Z_TYPE_P(helpers) == IS_OBJECT && Z_OBJ_HT_P(helpers)->get_properties != NULL ) {
        data_hash = Z_OBJ_HT_P(helpers)->get_properties(helpers TSRMLS_CC);
    } else {
        return;
    }

    // @todo merge with existing helpers?

    num = zend_hash_num_elements(data_hash);
    known_helpers = handlebars_talloc_array(compiler, char *, num + 1);

    do {
#ifdef ZEND_ENGINE_3
        zend_string *key;
        zend_ulong index;
        ZEND_HASH_FOREACH_KEY(data_hash, index, key) {
            if( key ) {
                known_helpers[idx++] = handlebars_talloc_strndup(known_helpers, ZSTR_VAL(key), ZSTR_LEN(key));
            }
        } ZEND_HASH_FOREACH_END();
#else
        char * key;
        int key_len;
        long index;
        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
            if (zend_hash_get_current_key_ex(data_hash, &key, &key_len, &index, 0, &data_pointer) == HASH_KEY_IS_STRING) {
                known_helpers[idx++] = handlebars_talloc_strndup(known_helpers, key, key_len);
            }
            zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
#endif
    } while(0);

    known_helpers[idx++] = 0;
    compiler->known_helpers = known_helpers;
}
/* }}} php_handlebars_fetch_known_helpers */

PHP_METHOD(HandlebarsVM, setHelpers)
{
    zval * _this_zval;
    zval * helpers;
    jmp_buf buf;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "OO",
            &_this_zval, HandlebarsVM_ce_ptr, &helpers, HandlebarsRegistry_ce_ptr) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(helpers, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();
#endif

    struct php_handlebars_vm_obj * intern = Z_HANDLEBARS_VM_P(_this_zval);
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, intern->context, &buf);
    if( intern->helpers ) {
        handlebars_value_dtor(intern->helpers);
    }
    intern->vm->helpers = intern->helpers = handlebars_value_from_zval(HBSCTX(intern->context), helpers TSRMLS_CC);
    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("helpers"), helpers TSRMLS_CC);
done:
    intern->context->jmp = NULL;
}

PHP_METHOD(HandlebarsVM, setPartials)
{
    zval * _this_zval;
    zval * partials;
    jmp_buf buf;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "OO",
            &_this_zval, HandlebarsVM_ce_ptr, &partials, HandlebarsRegistry_ce_ptr) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_OBJECT_OF_CLASS(partials, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();
#endif

    struct php_handlebars_vm_obj * intern = Z_HANDLEBARS_VM_P(_this_zval);
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, intern->context, &buf);
    if( intern->partials ) {
        handlebars_value_dtor(intern->partials);
    }
    intern->vm->partials = intern->partials = handlebars_value_from_zval(HBSCTX(intern->context), partials TSRMLS_CC);
    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("partials"), partials TSRMLS_CC);
done:
    intern->context->jmp = NULL;
}

PHP_METHOD(HandlebarsVM, render)
{
    zval * _this_zval;
    char * tmpl_str;
    strsize_t tmpl_len;
    zval * z_context = NULL;
    zval * z_options = NULL;
    zval * z_partials;
    void * mctx = NULL;
    struct handlebars_context * ctx = NULL;
    struct handlebars_parser * parser;
    struct handlebars_compiler * compiler;
    struct handlebars_vm * vm;
    struct handlebars_value * context;
    zend_long pool_size = HANDLEBARS_G(pool_size);
    jmp_buf buf;
    jmp_buf buf2;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|zz",
            &_this_zval, HandlebarsVM_ce_ptr, &tmpl_str, &tmpl_len, &z_context, &z_options) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STRING(tmpl_str, tmpl_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(z_context)
        Z_PARAM_ZVAL(z_options)
    ZEND_PARSE_PARAMETERS_END();
#endif

#if PHP_MAJOR_VERSION >= 7
    // Dereference zval
    if( Z_TYPE_P(z_context) == IS_REFERENCE ) {
        ZVAL_DEREF(z_context);
    }
#endif

    struct php_handlebars_vm_obj * intern = Z_HANDLEBARS_VM_P(_this_zval);
    vm = intern->vm;

    struct handlebars_string * tmpl = handlebars_string_ctor(HBSCTX(vm), tmpl_str, tmpl_len);

    // Lookup cache entry
    struct handlebars_cache_entry * cache_entry = handlebars_cache_find(HANDLEBARS_G(cache), tmpl);
    if( cache_entry ) {
        compiler = cache_entry->compiler;
    } else {
        ctx = handlebars_context_ctor_ex(HANDLEBARS_G(root));
        php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
        parser = handlebars_parser_ctor(ctx);
        compiler = handlebars_compiler_ctor(ctx);

        // Parse
        php_handlebars_try(HandlebarsParseException_ce_ptr, parser, &buf);
        parser->tmpl = tmpl;
        handlebars_parse(parser);

        // Compile
        php_handlebars_try(HandlebarsCompileException_ce_ptr, compiler, &buf);
        php_handlebars_process_options_zval(compiler, vm, z_options);
        /*if( z_helpers ) {
            php_handlebars_fetch_known_helpers(compiler, z_helpers TSRMLS_CC);
        }*/
        handlebars_compiler_compile(compiler, parser->program);

        // Save cache entry
        handlebars_cache_add(HANDLEBARS_G(cache), tmpl, compiler);
    }

    // Make context
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, intern->context, &buf2);
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, vm, &buf);
    context = handlebars_value_from_zval(HBSCTX(vm), z_context TSRMLS_CC);

    // Execute
    vm->flags = compiler->flags;
    handlebars_vm_execute(vm, compiler, context);

    if( vm->buffer ) { // @todo this probably shouldn't be null?
        PHP5TO7_RETVAL_STRING(vm->buffer);
    }

done:
    if( ctx ) {
        handlebars_context_dtor(ctx);
    }
    talloc_free(mctx);

    talloc_free_children(intern->vm); // potentially dangerous
    vm->ctx.jmp = NULL;
    intern->context->jmp = NULL;
}

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_setHelpers_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, helpers, Handlebars\\Registry, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_setPartials_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, partials, Handlebars\\Registry, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
    ZEND_ARG_INFO(0, context)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsVM methods */
static zend_function_entry HandlebarsCompiler_methods[] = {
    PHP_ME(HandlebarsVM, setHelpers, HandlebarsVM_setHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, setPartials, HandlebarsVM_setPartials_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, render, HandlebarsVM_render_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} HandlebarsVM methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_vm)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;

    memcpy(&HandlebarsVM_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
#ifdef ZEND_ENGINE_3
    HandlebarsVM_obj_handlers.offset = XtOffsetOf(struct php_handlebars_vm_obj, std);
    HandlebarsVM_obj_handlers.free_obj = php_handlebars_vm_obj_free;
#endif
    HandlebarsVM_obj_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Handlebars\\VM", HandlebarsCompiler_methods);
    HandlebarsVM_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    HandlebarsVM_ce_ptr ->create_object = php_handlebars_vm_obj_create;

    zend_declare_property_null(HandlebarsVM_ce_ptr, ZEND_STRL("helpers"), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(HandlebarsVM_ce_ptr, ZEND_STRL("partials"), ZEND_ACC_PROTECTED TSRMLS_CC);

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
