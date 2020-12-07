
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_compile.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_types.h"
#include "main/php.h"

#include <assert.h>
#include <talloc.h>

#define HANDLEBARS_HELPERS_PRIVATE

#include "handlebars.h"
#include "handlebars_memory.h"
#include "handlebars_helpers.h"
#include "handlebars_string.h"
#include "handlebars_value.h"
#include "handlebars_value_handlers.h"

#include "php_handlebars.h"

#include "php7to8.h"



/* {{{ Variables & Prototypes */
struct handlebars_zval {
    struct handlebars_user user;
    short callable;
    short int_array;
    zend_fcall_info_cache fcc;
    zval intern;
};
static struct handlebars_value_handlers handlebars_value_std_zval_handlers;

#define CHECK_INTERN(intern) if (!intern) zend_error_noreturn(E_ERROR, "Null intern in %s:%d", __FILE__, __LINE__)
/* }}} Variables & Prototypes */

/* {{{ Utils */
HBS_ATTR_NONNULL_ALL
static int handlebars_zval_intern_dtor(struct handlebars_zval * intern) {
    zval_ptr_dtor(&intern->intern);
    return 0;
}

HBS_ATTR_RETURNS_NONNULL HBS_ATTR_NONNULL_ALL
static inline zval * get_intern_zval(struct handlebars_value * value) {
    struct handlebars_zval * obj = (struct handlebars_zval *) handlebars_value_get_user(value);
    PHP_HBS_ASSERT(obj);
    return &obj->intern;
}

HBS_ATTR_NONNULL_ALL
static inline void set_intern_zval(struct handlebars_value * value, zval * val, struct handlebars_context * context) {
    struct handlebars_zval * obj = (struct handlebars_zval *) handlebars_value_get_user(value);
    if( !obj ) {
        obj = handlebars_talloc_zero(context, struct handlebars_zval);
        obj->user.handlers = &handlebars_value_std_zval_handlers;
        handlebars_value_user(value, (struct handlebars_user *) obj);
        talloc_set_destructor(obj, handlebars_zval_intern_dtor);
    }
    obj->int_array = -1;
    obj->callable = -1;
    ZVAL_ZVAL(&obj->intern, val, 1, 0);
}
/* }}} Utils */

/* {{{ handlebars zval value handlers */
static struct handlebars_value * handlebars_std_zval_copy(struct handlebars_value * value)
{
    // handlebars_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
    fprintf(stderr, "handlebars_std_zval_copy is not implemented");
    abort();
}

static void handlebars_std_zval_dtor(struct handlebars_user * user)
{
    //zval * intern = get_intern_zval(value);
    //Z_DELREF_P(intern);
    //zval_ptr_dtor(&intern);
}

HBS_ATTR_NORETURN
static void handlebars_std_zval_convert(struct handlebars_value * value, bool recurse)
{
    // handlebars_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
    fprintf(stderr, "handlebars_std_zval_convert is not implemented");
    abort();
}

static enum handlebars_value_type handlebars_std_zval_type(struct handlebars_value * value)
{
    //struct handlebars_zval * obj = talloc_get_type(value->v.user, struct handlebars_zval);
    struct handlebars_zval * obj = (struct handlebars_zval *) handlebars_value_get_user(value);
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

        default: // LCOV_EXCL_START
            fprintf(stderr, "Unimplemented handlebars value type for: %d", Z_TYPE_P(intern));
            abort();
            // handlebars_throw(value->ctx, HANDLEBARS_ERROR, "Unimplemented handlebars value type for: %d", Z_TYPE_P(intern));
            break; // LCOV_EXCL_STOP
    }
    return HANDLEBARS_VALUE_TYPE_NULL;
}

static struct handlebars_value * handlebars_std_zval_map_find(struct handlebars_value * value, struct handlebars_string * key, struct handlebars_value * rv)
{
    struct handlebars_context * ctx = ((struct handlebars_zval *) handlebars_value_get_user(value))->user.ctx;
    zval * intern = get_intern_zval(value);
    zval * entry = NULL;
    zval rval = {0};

    if (!intern) {
        zend_error_noreturn(E_ERROR, "Null intern in %s:%d", __FILE__, __LINE__);
    }

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            entry = zend_hash_str_find(Z_ARRVAL_P(intern), HBS_STR_STRL(key));
            if( !entry ) {
                char * end;
                long index = strtol(hbs_str_val(key), &end, 10);
                if( !*end ) {
                    entry = zend_hash_index_find(Z_ARRVAL_P(intern), index);
                }
            }
            break;
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(intern), zend_ce_arrayaccess) ) {
                zval zrv;
                zval prop;
                HBS_ZVAL_STR(&prop, key);
                ZVAL_UNDEF(&zrv);
#if PHP_MAJOR_VERSION >= 8
                if( Z_OBJ_HT_P(intern)->has_dimension(Z_OBJ_P(intern), &prop, 0) ) {
                    entry = Z_OBJ_HT_P(intern)->read_dimension(Z_OBJ_P(intern), &prop, 0, &zrv);
                }
#else
                if( Z_OBJ_HT_P(intern)->has_dimension(intern, &prop, 0) ) {
                    entry = Z_OBJ_HT_P(intern)->read_dimension(intern, &prop, 0, &zrv);
                }
#endif
                zval_ptr_dtor(&prop);
                if( entry ) {
                    rv = handlebars_value_from_zval(ctx, entry, rv);
                    zval_ptr_dtor(&zrv);
                    return rv;
                }
            } else {
                entry = zend_read_property(Z_OBJCE_P(intern), PHP7TO8_Z_OBJ_P(intern), HBS_STR_STRL(key), 1, NULL);
            }
            if( !entry || Z_TYPE_P(entry) == IS_NULL ) {
                char *error;
                zend_fcall_info_cache fcc;
                zend_function *mptr;
                zval callable;

                array_init(&callable);
                add_next_index_zval(&callable, intern);
                add_next_index_stringl(&callable, HBS_STR_STRL(key));

                if( zend_is_callable_ex(&callable, NULL, 0, NULL, &fcc, &error) ) {
                    mptr = fcc.function_handler;
                    zend_create_fake_closure(&rval, mptr, mptr->common.scope, fcc.called_scope, intern);
                    entry = &rval;
                }
            }
            break;

        default: assert(0); break; // LCOV_EXCL_LINE
    }

    if( entry != NULL ) {
        rv = handlebars_value_from_zval(ctx, entry, rv);
    } else {
        rv = NULL;
    }

    return rv;
}

static struct handlebars_value * handlebars_std_zval_array_find(struct handlebars_value * value, size_t index, struct handlebars_value * rv)
{
    struct handlebars_context * ctx = ((struct handlebars_zval *) handlebars_value_get_user(value))->user.ctx;
    zval * intern = get_intern_zval(value);
    zval * entry = NULL;

    CHECK_INTERN(intern);

    if( Z_TYPE_P(intern) == IS_ARRAY ) {
        entry = zend_hash_index_find(Z_ARRVAL_P(intern), index);
    }

    if( entry != NULL ) {
        rv = handlebars_value_from_zval(ctx, entry, rv);
    } else {
        rv = NULL;
    }

    return rv;
}

static bool handlebars_std_zval_iterator_void(struct handlebars_value_iterator * it)
{
    return false;
}

struct array_it_usr {
    HashTable * ht;
    HashPosition data_pointer;
};

static bool handlebars_std_zval_iterator_array(struct handlebars_value_iterator * it)
{
    struct handlebars_value * value = it->value;
    struct handlebars_context * ctx = ((struct handlebars_zval *) handlebars_value_get_user(value))->user.ctx;
    struct array_it_usr * itusr = it->usr;
    HashTable * ht = itusr->ht;
    HashPosition * data_pointer = &itusr->data_pointer;

    if( it->key ) {
        handlebars_string_delref(it->key);
        it->key = NULL;
    }

    zval * entry = zend_hash_get_current_data_ex(ht, data_pointer);
    if (!entry) {
        goto finished;
    }

    zend_string *string_key;
    zend_ulong num_key;
    switch (zend_hash_get_current_key_ex(ht, &string_key, &num_key, data_pointer)) {
        case HASH_KEY_NON_EXISTENT:
            goto finished;

        case HASH_KEY_IS_STRING:
            it->key = handlebars_string_ctor(ctx, ZSTR_VAL(string_key), ZSTR_LEN(string_key));
            handlebars_string_addref(it->key);
            it->index = 0;
            break;

        case HASH_KEY_IS_LONG:
            it->key = NULL;
            it->index = num_key;
            break;

        default: assert(0); break; // LCOV_EXCL_LINE
    }

    it->cur = handlebars_value_from_zval(ctx, entry, it->cur);
    zend_hash_move_forward_ex(ht, data_pointer);

    return true;

finished:
    handlebars_talloc_free(it->usr);
    handlebars_value_dtor(it->cur);
    it->usr = NULL;
    it->next = &handlebars_std_zval_iterator_void;
    return false;
}

static bool handlebars_std_zval_iterator_object(struct handlebars_value_iterator * it)
{
    struct handlebars_value * value = it->value;
    struct handlebars_context * ctx = ((struct handlebars_zval *) handlebars_value_get_user(value))->user.ctx;
    zend_object_iterator * zit = it->usr;
    zval *val;
    zval key;

    if( it->key ) {
        handlebars_string_delref(it->key);
        it->key = NULL;
    }

    if (zit->funcs->valid(zit) != SUCCESS) {
        goto iterator_err;
    }

    if (EG(exception)) {
        goto iterator_err;
    }

    val = zit->funcs->get_current_data(zit);
    if (EG(exception)) {
        goto iterator_err;
    }

    it->cur = handlebars_value_from_zval(ctx, val, it->cur);

    if (zit->funcs->get_current_key) {
        zit->funcs->get_current_key(zit, &key);
        if (EG(exception)) {
            goto iterator_err;
        }
        convert_to_string(&key);
        it->key = handlebars_string_ctor(ctx, Z_STRVAL(key), Z_STRLEN(key));
        zval_ptr_dtor(&key);
        handlebars_string_addref(it->key);
    } else {
        it->key = NULL;
        it->index++;
    }

    zit->funcs->move_forward(zit);
    if (EG(exception)) {
        goto iterator_err;
    }

    return true;

iterator_err:
    OBJ_RELEASE(&zit->std);
    handlebars_value_dtor(it->cur);
    it->usr = NULL;
    it->next = &handlebars_std_zval_iterator_void;
    return false;
}

bool handlebars_std_zval_iterator_init(struct handlebars_value_iterator * it, struct handlebars_value * value)
{
    struct handlebars_context * ctx = ((struct handlebars_zval *) handlebars_value_get_user(value))->user.ctx;
    zval * intern = get_intern_zval(value);
    struct array_it_usr * itusr;

    CHECK_INTERN(intern);

    it->value = value;

    switch( Z_TYPE_P(intern) ) {
        case IS_OBJECT:
            if (instanceof_function(Z_OBJCE_P(intern), zend_ce_traversable)) {
                zend_object_iterator * iter = Z_OBJCE_P(intern)->get_iterator(Z_OBJCE_P(intern), intern, 0);
                if (iter->funcs->rewind) {
                    iter->funcs->rewind(iter);
                    if (EG(exception)) {
                        OBJ_RELEASE(&iter->std);
                        goto done;
                    }
                }
                it->usr = (void *) iter;
                it->next = &handlebars_std_zval_iterator_object;
                return handlebars_std_zval_iterator_object(it);
            } else if (Z_OBJ_HT_P(intern)->get_properties) {
                itusr = handlebars_talloc_zero(ctx, struct array_it_usr);
#if PHP_MAJOR_VERSION >= 8
                itusr->ht = Z_OBJ_HT_P(intern)->get_properties(Z_OBJ_P(intern));
#else
                itusr->ht = Z_OBJ_HT_P(intern)->get_properties(intern);
#endif
                it->usr = (void *) itusr;
                it->next = &handlebars_std_zval_iterator_array;
                zend_hash_internal_pointer_reset_ex(itusr->ht, &itusr->data_pointer);
                return handlebars_std_zval_iterator_array(it);
            }
            break;

        case IS_ARRAY: {
            itusr = handlebars_talloc_zero(ctx, struct array_it_usr);
            itusr->ht = Z_ARRVAL_P(intern);
            it->usr = (void *) itusr;
            it->next = &handlebars_std_zval_iterator_array;
            zend_hash_internal_pointer_reset_ex(itusr->ht, &itusr->data_pointer);
            return handlebars_std_zval_iterator_array(it);
        }

        default:
            // Do nothing
            break;
    }

done:
    it->next = &handlebars_std_zval_iterator_void;
    return false;
}

long handlebars_std_zval_count(struct handlebars_value * value)
{
    zval * intern = get_intern_zval(value);

    CHECK_INTERN(intern);

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            return zend_hash_num_elements(Z_ARRVAL_P(intern));
        case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(intern), zend_ce_countable)) {
                zval retval = {0};
#if PHP_MAJOR_VERSION >= 8
				zend_call_method_with_0_params(Z_OBJ_P(intern), NULL, NULL, "count", &retval);
#else
				zend_call_method_with_0_params(intern, NULL, NULL, "count", &retval);
#endif
				if (Z_TYPE(retval) != IS_UNDEF) {
					return zval_get_long(&retval);
					zval_ptr_dtor(&retval);
				}
			}
            // fallthrough
        default:
            return -1;
    }

}

HANDLEBARS_FUNCTION_ATTRS
struct handlebars_value * handlebars_std_zval_call(struct handlebars_value * value, HANDLEBARS_FUNCTION_ARGS)
{
    //struct handlebars_zval * obj = talloc_get_type(value->v.usr, struct handlebars_zval);
    struct handlebars_zval * obj = (struct handlebars_zval *) handlebars_value_get_user(value);
    zval * intern = get_intern_zval(value);
    zval * z_ret;
    zend_function *fptr;
	zend_class_entry *ce = NULL;
	int num_args;
	struct _zend_arg_info *arg_info;
    short send_options = 0;

    CHECK_INTERN(intern);

    // Check if is callable object (closure or __invoke)
    if( !intern || Z_TYPE_P(intern) != IS_OBJECT ) {
        return rv;
    }

    if( obj->callable == -1 ) {
        obj->callable = php_handlebars_is_callable(intern);
    }

    if( !obj->callable ) {
        return rv;
    }

    // If the argument is a closure: we will omit passing the options
    // object if it will go into a slot with an incompatible type
    // If the argument is a callable object: we will append the
    // options if the appended parameter will go into a
    // declared parameter with either no type signature or explicitly
    // typed with Handlebars\Options
    ce = Z_OBJCE_P(intern);
    if (instanceof_function(ce, zend_ce_closure)) {
        fptr = (zend_function *)zend_get_closure_method_def(PHP7TO8_Z_OBJ_P(intern));
        send_options = 1;
    } else {
        fptr = zend_hash_find_ptr(&ce->function_table, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE));
    }
    if (fptr) {
        arg_info = fptr->common.arg_info;
        num_args = fptr->common.num_args;

        if (argc < num_args) {
// Current TravisCI's PHP master is really old, falling through to PHP 7 case should fix it...
#if PHP_VERSION_ID >= 80000 && defined(ZEND_TYPE_IS_ONLY_MASK)
            zend_type type = (arg_info + argc)->type;
            zend_type *subtype;

            if (ZEND_TYPE_IS_ONLY_MASK(type)) {
                send_options = 0;
            }

            ZEND_TYPE_FOREACH(type, subtype) {
                if (ZEND_TYPE_HAS_NAME(*subtype)) {
                    if (0 == strcmp(ZSTR_VAL(ZEND_TYPE_NAME(type)), "Handlebars\\Options")) {
                        send_options = 1;
                    }
                }
            } ZEND_TYPE_FOREACH_END();
#else
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
#endif
        }
    }

    // Convert paramsma
    int n_args = argc + (send_options ? 1 : 0);
    zval *z_args = alloca(sizeof(zval) * n_args);
    memset(z_args, 0, sizeof(zval) * n_args);

    int i;
    for( i = 0; i < argc; i++ ) {
        handlebars_value_to_zval(HANDLEBARS_ARG_AT(i), &z_args[i]);
    }

    if (send_options) {
        zval z_options;
        php_handlebars_options_ctor(vm, options, &z_options);
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
#if PHP_MAJOR_VERSION < 8
    fci.no_separation = 1;
#endif

    ZVAL_STRING(&fci.function_name, "__invoke");

    if( zend_call_function(&fci, &obj->fcc) == FAILURE ) {
        zend_throw_exception_ex(zend_ce_exception, 0, "Could not execute %s::%s()", ZSTR_VAL(Z_OBJCE_P(intern)->name), ZSTR_VAL(Z_OBJCE_P(intern)->constructor->common.function_name));
    }

    for( i = 0; i < n_args; i++ ) {
        zval_ptr_dtor(&z_args[i]);
    }
    zval_ptr_dtor(&fci.function_name);

    if( Z_TYPE_P(z_ret) == IS_OBJECT && instanceof_function(Z_OBJCE_P(z_ret), HandlebarsSafeString_ce_ptr) ) {
        convert_to_string(z_ret);
        rv = handlebars_value_from_zval(HBSCTX(vm), z_ret, rv);
        handlebars_value_set_flag(rv, HANDLEBARS_VALUE_FLAG_SAFE_STRING);
    } else {
        rv = handlebars_value_from_zval(HBSCTX(vm), z_ret, rv);
    }

    zval_ptr_dtor(z_ret);

    if( EG(exception) ) {
        handlebars_throw(HBSCTX(vm), HANDELBARS_EXTERNAL_ERROR, "external error");
    }

    return rv;
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
    array_init(val);

    HANDLEBARS_VALUE_FOREACH(value, child) {
        zval tmp = {0};
        handlebars_value_to_zval(child, &tmp);
        add_next_index_zval(val, &tmp);
    } HANDLEBARS_VALUE_FOREACH_END();
}

static inline void handlebars_value_map_to_zval(struct handlebars_value * value, zval * val)
{
    array_init(val);

    HANDLEBARS_VALUE_FOREACH_KV(value, key, child) {
        zval tmp = {0};
        handlebars_value_to_zval(child, &tmp);
        add_assoc_zval_ex(val, HBS_STR_STRL(key), &tmp); // @todo does this need + 1
    } HANDLEBARS_VALUE_FOREACH_END();
}

PHP_HANDLEBARS_API zval * handlebars_value_to_zval(struct handlebars_value * value, zval * val)
{
    switch( handlebars_value_get_real_type(value) ) {
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
            ZVAL_DOUBLE(val, handlebars_value_get_floatval(value));
            break;
        case HANDLEBARS_VALUE_TYPE_INTEGER:
            ZVAL_LONG(val, handlebars_value_get_intval(value));
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
        case HANDLEBARS_VALUE_TYPE_USER: {
            zval * intern = get_intern_zval(value);
            CHECK_INTERN(intern);
            // @todo check to make sure it's a zval type
            ZVAL_ZVAL(val, intern, 1, 0);
            break;
        }

        case HANDLEBARS_VALUE_TYPE_CLOSURE: // LCOV_EXCL_START
        case HANDLEBARS_VALUE_TYPE_PTR:
        case HANDLEBARS_VALUE_TYPE_HELPER:
        default:
            assert(0);
            ZVAL_NULL(val);
            break; // LCOV_EXCL_STOP
    }

    return val;
}
/* }}} handlebars_value_to_zval */

/* {{{ handlebars_value_from_zval */
PHP_HANDLEBARS_API struct handlebars_value * handlebars_value_from_zval(struct handlebars_context * context, zval * val, struct handlebars_value * rv)
{
    switch( Z_TYPE_P(val) ) {
        case IS_UNDEF:
        case IS_NULL:
            // do nothing
            break;
        case IS_TRUE:
            handlebars_value_boolean(rv, 1);
            break;
        case IS_FALSE:
            handlebars_value_boolean(rv, 0);
            break;
        case IS_DOUBLE:
            handlebars_value_float(rv, Z_DVAL_P(val));
            break;
        case IS_LONG:
            handlebars_value_integer(rv, Z_LVAL_P(val));
            break;
        case IS_STRING: {
            handlebars_value_str(rv, handlebars_string_ctor(context, Z_STRVAL_P(val), Z_STRLEN_P(val)));
            break;
        }

        case IS_OBJECT:
            // fall-through
        case IS_ARRAY:
            set_intern_zval(rv, val, context);
            break;

        default: // LCOV_EXCL_START
            handlebars_throw(context, HANDLEBARS_ERROR, "Unimplemented handlebars value conversion for: %d", Z_TYPE_P(val));
            break; // LCOV_EXCL_STOP
    }

    return rv;
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
