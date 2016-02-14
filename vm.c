
#ifdef HAVE_CONFIG_H

#include <handlebars_value.h>
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
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



/* {{{ php_handlebars_process_options_zval */
long php_handlebars_process_options_zval(struct handlebars_compiler * compiler, struct handlebars_vm * vm, zval * options)
{
    zval * entry;
    HashTable * ht;
    long flags = 0;

    if( !options || Z_TYPE_P(options) != IS_ARRAY ) {
        return 0;
    }

    ht = Z_ARRVAL_P(options);
    if( NULL != (entry = php5to7_zend_hash_find(ht, ZEND_STRL("compat"))) ) {
        if( Z_BVAL_P(entry) ) {
            flags |= handlebars_compiler_flag_compat;
        }
    }
    if( NULL != (entry = php5to7_zend_hash_find(ht, ZEND_STRL("data"))) ) {
        // @todo refine this
        if( Z_TYPE_P(entry) != IS_BOOL ) {
            if( vm ) {
                vm->data = handlebars_value_from_zval(vm->ctx, entry);
            }
        } else if( Z_BVAL_P(entry) ) {
            flags |= handlebars_compiler_flag_use_data;
        }
    }

    handlebars_compiler_set_flags(compiler, flags);

}
/* }}} php_handlebars_process_options_zval */

/* {{{ handlebars zval value handlers */
static struct handlebars_value * handlebars_std_zval_copy(struct handlebars_value * value)
{
    // @todo
    return NULL;
//    const char * str = json_object_to_json_string(value->v.usr);
//    return handlebars_value_from_json_string(value->ctx, str);
}

static void handlebars_std_zval_dtor(struct handlebars_value * value)
{
    zval * intern = (zval *) value->v.usr;

//    struct json_object * result = (struct json_object *) value->v.usr;
//
//    assert(value->type == HANDLEBARS_VALUE_TYPE_USER);
//    assert(result != NULL);
//
//    if( result != NULL ) {
//        json_object_put(result);
//        value->v.usr = NULL;
//    }
}

static void handlebars_std_zval_convert(struct handlebars_value * value, bool recurse)
{
    // @todo
//    struct json_object * intern = (struct json_object *) value->v.usr;
//    char * key;
//    struct handlebars_value * new_value;
//
//    switch( json_object_get_type(intern) ) {
//        case json_type_object: {
//            handlebars_value_map_init(value);
//            json_object_object_foreach(intern, k, v) {
//                new_value = handlebars_value_from_json_object(value->ctx, v);
//                handlebars_map_add(value->v.map, k, new_value);
//                handlebars_value_delref(new_value);
//                if( recurse && new_value->type == HANDLEBARS_VALUE_TYPE_USER ) {
//                    handlebars_std_zval_convert(new_value, recurse);
//                }
//            }
//            break;
//        }
//        case json_type_array: {
//            handlebars_value_array_init(value);
//            size_t i, l;
//
//            for( i = 0, l = json_object_array_length(intern); i < l; i++ ) {
//                new_value = handlebars_value_from_json_object(value->ctx, json_object_array_get_idx(intern, i));
//                handlebars_stack_set(value->v.map, i, new_value);
//                handlebars_value_delref(new_value);
//                if( recurse && new_value->type == HANDLEBARS_VALUE_TYPE_USER ) {
//                    handlebars_std_zval_convert(new_value, recurse);
//                }
//            }
//            break;
//        }
//    }
//
//    // Remove talloc destructor?
//    talloc_set_destructor(value, NULL);
//    value->flags &= ~HANDLEBARS_VALUE_FLAG_TALLOC_DTOR;
}

static enum handlebars_value_type handlebars_std_zval_type(struct handlebars_value * value)
{
    zval * intern = (zval *) value->v.usr;
    if( Z_TYPE_P(intern) == IS_ARRAY ) {
        // @todo cache
        if( php_handlebars_is_int_array(intern) ) {
            return HANDLEBARS_VALUE_TYPE_ARRAY;
        } else {
            return HANDLEBARS_VALUE_TYPE_MAP;
        }
    } else if( Z_TYPE_P(intern) == IS_OBJECT ) {
        // @todo cache
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
    zval * prop;
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
            MAKE_STD_ZVAL(prop);
            ZVAL_STRINGL(prop, key, strlen(key), 1);
            if( Z_OBJ_HT_P(intern)->has_dimension(intern, prop, 0 TSRMLS_CC) ) {
                entry = Z_OBJ_HT_P(intern)->read_dimension(intern, prop, 0 TSRMLS_CC);
            }
            zval_ptr_dtor(&prop);
        } else {
            entry = zend_read_property(Z_OBJCE_P(intern), intern, key, strlen(key), 1 TSRMLS_CC);
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
    int key_type = 0;
    char * key_str = NULL;
    uint key_len = 0;
    ulong key_nindex = 0;
    TSRMLS_FETCH();

    it->value = value;

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            ht = HASH_OF(intern);
            data_pointer = handlebars_talloc_zero(value->ctx, HashPosition);
            it->usr = (void *) data_pointer;
            it->length = zend_hash_num_elements(ht);
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
    zval ** data_entry = NULL;
    HashPosition * data_pointer;
    HashTable * ht;
    int key_type = 0;
    char * key_str = NULL;
    uint key_len = 0;
    ulong key_nindex = 0;
    TSRMLS_FETCH();

    switch( Z_TYPE_P(intern) ) {
        case IS_ARRAY:
            ht = Z_ARRVAL_P(intern);
            data_pointer = (HashPosition *) it->usr;
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
    zval z_ret = {0};
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
    // @todo
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
    ZVAL_STRING(&z_const, "__invoke", 0);
    call_user_function(&Z_OBJCE_P(intern)->function_table, &intern, &z_const, &z_ret, n_args, z_const_args TSRMLS_CC);
    efree(z_const_args);

    struct handlebars_value * retval = NULL;
    bool is_safe_string = false;

    switch( Z_TYPE(z_ret) ) {
        case IS_OBJECT:
            if( instanceof_function(Z_OBJCE(z_ret), HandlebarsSafeString_ce_ptr TSRMLS_CC) ) {
                is_safe_string = true;
                goto scalar;
            }
            // fall-through
        case IS_ARRAY:
            retval = handlebars_value_from_zval(options->vm->ctx, &z_ret);
            break;

        default: // not ideal?
        case IS_NULL:
        case IS_BOOL:
        case IS_LONG:
        case IS_DOUBLE:
        case IS_STRING: scalar:
            retval = handlebars_value_ctor(value->ctx);
            convert_to_string(&z_ret);
            handlebars_value_stringl(retval, Z_STRVAL(z_ret), Z_STRLEN(z_ret));
            break;
    }

    if( is_safe_string ) {
        retval->flags |= HANDLEBARS_VALUE_FLAG_SAFE_STRING;
    }

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
        zval * tmp;
        MAKE_STD_ZVAL(tmp);
        handlebars_value_to_zval(it->current, tmp);
        add_next_index_zval(val, tmp);
    }
}

static inline handlebars_value_map_to_zval(struct handlebars_value * value, zval * val TSRMLS_DC)
{
    struct handlebars_value_iterator * it = handlebars_value_iterator_ctor(value);
    array_init(val);

    for( ; it->current != NULL; handlebars_value_iterator_next(it) ) {
        zval * tmp;
        MAKE_STD_ZVAL(tmp);
        handlebars_value_to_zval(it->current, tmp);
        add_assoc_zval_ex(val, it->key, talloc_array_length(it->key), tmp);
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
            ZVAL_STRINGL(val, value->v.strval, talloc_array_length(value->v.strval) - 1, 1);
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
            value->v.bval = true;
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
        case IS_ARRAY:
            //nzv = talloc_zero(context, zval);
            //nzv = ecalloc(1, sizeof(zval));
            MAKE_STD_ZVAL(nzv);
            ZVAL_ZVAL(nzv, val, 1, 0);
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
    struct handlebars_context * ctx;
    struct handlebars_compiler * compiler;
    struct handlebars_vm * vm;
    struct handlebars_value * context;
    char * errmsg;
    struct zend_class_entry * volatile exception_ce = HandlebarsRuntimeException_ce_ptr;

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
    ZEND_PARSE_PARAMETERS_END();
#endif

#if PHP_MAJOR_VERSION >= 7
    // Dereference zval
    if( Z_TYPE_P(z_context) == IS_REFERENCE ) {
        ZVAL_DEREF(z_context);
    }
#endif

    // Initialize
    ctx = handlebars_context_ctor();

    // Save jump buffer
    ctx->e.ok = true;
    if( setjmp(ctx->e.jmp) ) {
        errmsg = handlebars_context_get_errmsg(ctx);
        zend_throw_exception(exception_ce, errmsg, ctx->e.num TSRMLS_CC);
        goto done;
    }

    // Initialize
    compiler = handlebars_compiler_ctor(ctx);
    vm = handlebars_vm_ctor(ctx);

    // Parse
    exception_ce = HandlebarsParseException_ce_ptr;
    ctx->tmpl = tmpl;
    handlebars_parse(ctx);

    // Compile
    exception_ce = HandlebarsCompileException_ce_ptr;
    php_handlebars_process_options_zval(compiler, vm, z_options);
    handlebars_compiler_compile(compiler, ctx->program);

    // Make context
    context = handlebars_value_from_zval(ctx, z_context TSRMLS_CC);

    // Make VM
    vm->flags = compiler->flags;

    // Make helpers
    z_helpers = php5to7_zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("helpers"), 0);
    vm->helpers = handlebars_value_from_zval(ctx, z_helpers TSRMLS_CC);

    // Make partials
    z_partials = php5to7_zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("partials"), 0);
    vm->partials = handlebars_value_from_zval(ctx, z_partials TSRMLS_CC);

    // Execute
    handlebars_vm_execute(vm, compiler, context);

    if( vm->buffer ) { // @todo this probably shouldn't be null?
        PHP5TO7_RETVAL_STRING(vm->buffer);
    }

done:
    handlebars_context_dtor(ctx);
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
