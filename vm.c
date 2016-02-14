
#ifdef HAVE_CONFIG_H

#include <handlebars_value.h>
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"

#include "handlebars_private.h"

#include "handlebars.h"
#include "handlebars_compiler.h"
#include "handlebars_context.h"
#include "handlebars_helpers.h"
#include "handlebars_memory.h"
#include "handlebars_opcodes.h"
#include "handlebars_value.h"
#include "handlebars_value_handlers.h"
#include "handlebars_vm.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsVM_ce_ptr;
/* }}} Variables & Prototypes */




/* {{{ handlebars zval value handlers */
static struct handlebars_value * handlebars_std_zval_copy(struct handlebars_value * value)
{
    handlebars_context_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
    return NULL;
}

static void handlebars_std_zval_dtor(struct handlebars_value * value)
{
    zval * intern = (zval *) value->v.usr;
}

static void handlebars_std_zval_convert(struct handlebars_value * value, bool recurse)
{
    handlebars_context_throw(value->ctx, HANDLEBARS_ERROR, "handlebars_std_zval_convert is not implemented");
}

static enum handlebars_value_type handlebars_std_zval_type(struct handlebars_value * value)
{
    zval * intern = (zval *) value->v.usr;
    if( Z_TYPE_P(intern) == IS_ARRAY ) {
        // @todo cache?
        if( php_handlebars_is_int_array(intern) ) {
            return HANDLEBARS_VALUE_TYPE_ARRAY;
        } else {
            return HANDLEBARS_VALUE_TYPE_MAP;
        }
    } else if( Z_TYPE_P(intern) == IS_OBJECT ) {
        // @todo cache?
        if( php_handlebars_is_callable(intern) ) {
            return HANDLEBARS_VALUE_TYPE_HELPER;
        } else {
            return HANDLEBARS_VALUE_TYPE_MAP;
        }
    }
    assert(0);
    return HANDLEBARS_VALUE_TYPE_NULL;
}

static struct handlebars_value * handlebars_std_zval_map_find(struct handlebars_value * value, const char * key)
{
    zval * intern = (zval *) value->v.usr;
    zval * entry = NULL;
    struct handlebars_value * ret = NULL;
    TSRMLS_FETCH();

    if( Z_TYPE_P(intern) == IS_ARRAY ) {
        entry = php5to7_zend_hash_find(Z_ARRVAL_P(intern), key, strlen(key));
        if( !entry ) {
            char * end;
            long index = strtod(key, &end);
            if( !*end ) {
                entry = php5to7_zend_hash_index_find(Z_ARRVAL_P(intern), index);
            }
        }
    } else if( Z_TYPE_P(intern) == IS_OBJECT ) {
        if( instanceof_function(Z_OBJCE_P(intern), zend_ce_arrayaccess TSRMLS_CC) ) {
#ifdef ZEND_ENGINE_3
            zval prop;
            ZVAL_STRINGL(&prop, key, strlen(key));
            if( Z_OBJ_HT_P(intern)->has_dimension(intern, &prop, 0 TSRMLS_CC) ) {
                entry = Z_OBJ_HT_P(intern)->read_dimension(intern, &prop, 0, entry TSRMLS_CC);
            }
            zval_dtor(&prop);
#else
            zval * prop;
            MAKE_STD_ZVAL(prop);
            ZVAL_STRINGL(prop, key, strlen(key), 1);
            if( Z_OBJ_HT_P(intern)->has_dimension(intern, prop, 0 TSRMLS_CC) ) {
                entry = Z_OBJ_HT_P(intern)->read_dimension(intern, prop, 0 TSRMLS_CC);
            }
            zval_ptr_dtor(&prop);
#endif
        } else {
            entry = php5to7_zend_read_property2(Z_OBJCE_P(intern), intern, key, strlen(key), 1);
        }
    }

    if( entry != NULL ) {
        ret = handlebars_value_from_zval(value->ctx, entry TSRMLS_CC);
    }

    return ret;
}

static struct handlebars_value * handlebars_std_zval_array_find(struct handlebars_value * value, size_t index)
{
    zval * intern = (zval *) value->v.usr;
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
    struct handlebars_value_iterator * it = handlebars_talloc(value, struct handlebars_value_iterator);
    zval * intern = (zval *) value->v.usr;
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
    zval * intern = (zval *) value->v.usr;
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
    zval * intern = (zval *) value->v.usr;

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
    zval * intern = (zval *) value->v.usr;
    zend_bool is_callable = 0;
    int check_flags = 0; //IS_CALLABLE_CHECK_SYNTAX_ONLY;
    char * error;
    zval z_const = {0};
    zval * z_ret;
    TSRMLS_FETCH();

    // Check if is callable object (closure or __invoke)
    if( Z_TYPE_P(intern) != IS_OBJECT ) {
        return NULL;
    }

#if PHP_MAJOR_VERSION < 7
    is_callable = zend_is_callable_ex(intern, NULL, check_flags, NULL, NULL, NULL, &error TSRMLS_CC);
#else
    is_callable = zend_is_callable_ex(intern, NULL, check_flags, NULL, NULL, &error);
#endif

    if( !is_callable ) {
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
    }

    // Add hash
    if( options->hash ) {
        zval z_hash;
        handlebars_value_to_zval(options->hash, &z_hash);
        zend_update_property(Z_OBJCE(z_options), &z_options, ZEND_STRL("hash"), &z_hash);
    }

    // Add data
    if( options->data ) {
        zval z_data;
        handlebars_value_to_zval(options->data, &z_data);
        zend_update_property(Z_OBJCE(z_options), &z_options, ZEND_STRL("data"), &z_data TSRMLS_CC);
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

    zval _z_ret;
    z_ret = &_z_ret;
    ZVAL_UNDEF(z_ret);
    ZVAL_STRING(&z_const, "__invoke");
    call_user_function(&Z_OBJCE_P(intern)->function_table, intern, &z_const, z_ret, n_args, z_const_args TSRMLS_CC);

    zval_dtor(&z_const);
#else
    // Construct options
    zval * z_options;
    MAKE_STD_ZVAL(z_options);
    php_handlebars_options_ctor(options, z_options TSRMLS_CC);

    // Add scope
    if( options->scope ) {
        zval *z_scope;
        MAKE_STD_ZVAL(z_scope);
        handlebars_value_to_zval(options->scope, z_scope);
        zend_update_property(Z_OBJCE_P(z_options), z_options, ZEND_STRL("scope"), z_scope TSRMLS_CC);
    }

    // Add hash
    if( options->hash ) {
        zval *z_hash;
        MAKE_STD_ZVAL(z_hash);
        handlebars_value_to_zval(options->hash, z_hash);
        zend_update_property(Z_OBJCE_P(z_options), z_options, ZEND_STRL("hash"), z_hash TSRMLS_CC);
    }

    // Add data
    if( options->data ) {
        zval *z_data;
        MAKE_STD_ZVAL(z_data);
        handlebars_value_to_zval(options->data, z_data);
        zend_update_property(Z_OBJCE_P(z_options), z_options, ZEND_STRL("data"), z_data TSRMLS_CC);
    }

    // Convert params
    size_t n_args = handlebars_stack_length(options->params) + 1;
    zval **z_const_args = emalloc(n_args * sizeof(zval *));

    int i;
    for( i = 0; i < n_args - 1; i++ ) {
        struct handlebars_value * val = handlebars_stack_get(options->params, i);
        MAKE_STD_ZVAL(z_const_args[i]);
        handlebars_value_to_zval(val, z_const_args[i]);
    }

    z_const_args[n_args - 1] = z_options;

    // Call
    MAKE_STD_ZVAL(z_ret);
    ZVAL_STRING(&z_const, "__invoke", 0);
    call_user_function(&Z_OBJCE_P(intern)->function_table, &intern, &z_const, z_ret, n_args, z_const_args TSRMLS_CC);
    efree(z_const_args);
#endif

    struct handlebars_value * retval = NULL;
    bool is_safe_string = false;

    switch( Z_TYPE_P(z_ret) ) {
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE_P(z_ret), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
                convert_to_string(z_ret);
                retval = handlebars_value_from_zval(options->vm->ctx, z_ret);
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
            retval = handlebars_value_from_zval(options->vm->ctx, z_ret);
            break;
        default:
            break;
    }

    // @todo this seems to cause a problem
    //zval_ptr_dtor(z_ret);

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
        handlebars_value_to_zval(it->current, tmp);
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
        add_assoc_zval_ex(val, it->key, talloc_array_length(it->key) - 1, tmp);
#else
        zval * tmp;
        MAKE_STD_ZVAL(tmp);
        handlebars_value_to_zval(it->current, tmp);
        add_assoc_zval_ex(val, it->key, talloc_array_length(it->key), tmp);
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
            PHP5TO7_ZVAL_STRINGL(val, value->v.strval, talloc_array_length(value->v.strval) - 1);
            break;
        case HANDLEBARS_VALUE_TYPE_ARRAY:
            handlebars_value_array_to_zval(value, val TSRMLS_CC);
            break;
        case HANDLEBARS_VALUE_TYPE_MAP:
            handlebars_value_map_to_zval(value, val TSRMLS_CC);
            break;
        case HANDLEBARS_VALUE_TYPE_USER:
            intern = (zval *) value->v.usr;
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
    zval * nzv;

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
            value->v.strval = MC(handlebars_talloc_strndup(value, Z_STRVAL_P(val), Z_STRLEN_P(val)));
            break;

        case IS_OBJECT:
            /*
            if( instanceof_function(Z_OBJCE_P(val), HandlebarsSafeString_ce_ptr) ) {
                convert_to_string(val);
                value->type = HANDLEBARS_VALUE_TYPE_STRING;
                value->v.strval = MC(handlebars_talloc_strndup(value, Z_STRVAL_P(val), Z_STRLEN_P(val)));
                value->flags |= HANDLEBARS_VALUE_FLAG_SAFE_STRING;
                break;
            }
            */
            // fall-through
        case IS_ARRAY:
#ifdef ZEND_ENGINE_3
            nzv = talloc_zero(value, zval); // fear
            ZVAL_ZVAL(nzv, val, 1, 0);
#else
            MAKE_STD_ZVAL(nzv);
            ZVAL_ZVAL(nzv, val, 1, 0);
#endif
            // @todo destructor?

            value->type = HANDLEBARS_VALUE_TYPE_USER;
            value->handlers = &handlebars_value_std_zval_handlers;
            value->v.usr = (void *) nzv;
            // Increment refcount?

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
void php_handlebars_fetch_known_helpers(struct handlebars_compiler * compiler, zval * helpers)
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
    known_helpers = handlebars_talloc_array(compiler->ctx, char *, num + 1);

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
/* }}} handlebars_value_from_zval */

PHP_METHOD(HandlebarsVM, setHelpers)
{
    zval * _this_zval;
    zval * helpers;

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

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("helpers"), helpers TSRMLS_CC);
}

PHP_METHOD(HandlebarsVM, setPartials)
{
    zval * _this_zval;
    zval * partials;

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

    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("partials"), partials TSRMLS_CC);
}

PHP_METHOD(HandlebarsVM, render)
{
    zval * _this_zval;
    char * tmpl;
    strsize_t tmpl_len;
    zval * z_context;
    zval * z_options;
    zval * z_helpers;
    zval * z_partials;
    void * mctx = NULL;
    struct handlebars_context * ctx;
    struct handlebars_compiler * compiler;
    struct handlebars_vm * vm;
    struct handlebars_value * context;
    char * errmsg;
    volatile struct {
        zend_class_entry * ce;
    } ex;
    zend_long pool_size = HANDLEBARS_G(pool_size);

    ex.ce = HandlebarsRuntimeException_ce_ptr;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "Os|zz",
            &_this_zval, HandlebarsVM_ce_ptr, &tmpl, &tmpl_len, &z_context, &z_options) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STRING(tmpl, tmpl_len)
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

    // Initialize
    if( pool_size <= 0 ) {
        ctx = handlebars_context_ctor();
    } else {
        mctx = talloc_pool(NULL, pool_size);
        ctx = handlebars_context_ctor_ex(mctx);
    }

    // Save jump buffer
    ctx->e.ok = true;
    if( setjmp(ctx->e.jmp) ) {
        zend_throw_exception(ex.ce, handlebars_context_get_errmsg(ctx), ctx->e.num TSRMLS_CC);
        goto done;
    }

    // Initialize
    compiler = handlebars_compiler_ctor(ctx);
    vm = handlebars_vm_ctor(ctx);

    // Make context
    context = handlebars_value_from_zval(ctx, z_context TSRMLS_CC);

    // Make helpers
    z_helpers = php5to7_zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("helpers"), 0);
    vm->helpers = handlebars_value_from_zval(ctx, z_helpers TSRMLS_CC);

    // Make partials
    z_partials = php5to7_zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("partials"), 0);
    vm->partials = handlebars_value_from_zval(ctx, z_partials TSRMLS_CC);

    // Process options
    php_handlebars_process_options_zval(compiler, vm, z_options);
    vm->flags = compiler->flags;

    // Process known helpers
    if( z_helpers ) {
        php_handlebars_fetch_known_helpers(compiler, z_helpers);
    }

    // Parse
    ex.ce = HandlebarsParseException_ce_ptr;
    ctx->tmpl = tmpl;
    handlebars_parse(ctx);

    // Compile
    ex.ce = HandlebarsCompileException_ce_ptr;
    handlebars_compiler_compile(compiler, ctx->program);

    // Execute
    ex.ce = HandlebarsRuntimeException_ce_ptr;
    handlebars_vm_execute(vm, compiler, context);

    if( vm->buffer ) { // @todo this probably shouldn't be null?
        PHP5TO7_RETVAL_STRING(vm->buffer);
    }

done:
    handlebars_context_dtor(ctx);
    talloc_free(mctx);
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

    INIT_CLASS_ENTRY(ce, "Handlebars\\VM", HandlebarsCompiler_methods);
    HandlebarsVM_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

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
