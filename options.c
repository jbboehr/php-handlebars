
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <talloc.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "main/php.h"

#include "php5to7.h"
#include "php_handlebars.h"

#include "handlebars_context.h"
#include "handlebars_helpers.h"
#include "handlebars_value.h"
#include "handlebars_vm.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsOptions_ce_ptr;
static zend_object_handlers HandlebarsOptions_obj_handlers;
/* }}} Variables & Prototypes */

struct php_handlebars_options_obj {
#if PHP_MAJOR_VERSION < 7
    zend_object std;
#endif
    struct handlebars_vm * vm;
    long program;
    long inverse;
#if PHP_MAJOR_VERSION >= 7
    zend_object std;
#endif
};

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsOptions_construct_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, props, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsOptions_offsetExists_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_INFO(0, prop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(HandlebarsOptions_offsetSet_args, ZEND_SEND_BY_VAL, 0, 2)
    ZEND_ARG_INFO(0, prop)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ Z_HANDLEBARS_OPTIONS_P */
#ifdef ZEND_ENGINE_3
static inline struct php_handlebars_options_obj * php_handlebars_options_fetch_object(zend_object *obj) {
    return (struct php_handlebars_options_obj *)((char*)(obj) - XtOffsetOf(struct php_handlebars_options_obj, std));
}
#define Z_HBS_OPTIONS_P(zv) php_handlebars_options_fetch_object(Z_OBJ_P((zv)))
#else
#define Z_HBS_OPTIONS_P(zv) zend_object_store_get_object(zv TSRMLS_CC)
#endif
/* }}} */

/* {{{ php_handlebars_options_obj_free */
#ifdef ZEND_ENGINE_3
static void php_handlebars_options_obj_free(zend_object * object TSRMLS_DC)
{
    struct php_handlebars_options_obj * payload = php_handlebars_options_fetch_object(object TSRMLS_CC);
    zend_object_std_dtor((zend_object *)object TSRMLS_CC);
}
#else
static void php_handlebars_options_obj_free(void *object TSRMLS_DC)
{
    struct php_handlebars_options_obj * payload = (struct php_handlebars_options_obj *) object;

    zend_object_std_dtor(&payload->std TSRMLS_CC);
    efree(object);
}
#endif
/* }}} */

/* {{{ php_handlebars_options_obj_create */
#ifdef ZEND_ENGINE_3
static zend_object * php_handlebars_options_obj_create(zend_class_entry * ce)
{
    struct php_handlebars_options_obj *obj;

    obj = ecalloc(1, sizeof(*obj) + zend_object_properties_size(ce));
    zend_object_std_init(&obj->std, ce);
    object_properties_init(&obj->std, ce);
    obj->std.handlers = &HandlebarsOptions_obj_handlers;

    obj->vm = NULL;

    return &obj->std;
}
#else
zend_object_value php_handlebars_options_obj_create(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    struct php_handlebars_options_obj *obj;
    zval *tmp;

    obj = ecalloc(1, sizeof(struct php_handlebars_options_obj));
	zend_object_std_init(&obj->std, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_property_ctor, (void *) &tmp, sizeof(zval *));
#else
	object_properties_init(&obj->std, ce);
#endif

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)php_handlebars_options_obj_free, NULL TSRMLS_CC);
    retval.handlers = &HandlebarsOptions_obj_handlers;

    obj->vm = NULL;

    return retval;
}
#endif
/* }}} */

/* {{{ php_handlebars_options_ctor */
PHPAPI void php_handlebars_options_ctor(struct handlebars_options * options, zval * z_options/*, long program, long inverse */TSRMLS_DC)
{
    struct php_handlebars_options_obj * intern;

    object_init_ex(z_options, HandlebarsOptions_ce_ptr);

    do {
#if PHP_MAJOR_VERSION < 7
        zval * z_const;
        zval * z_ret;
        MAKE_STD_ZVAL(z_const);
        MAKE_STD_ZVAL(z_ret);
        ZVAL_STRING(z_const, "__construct", 1);
        call_user_function(&HandlebarsOptions_ce_ptr->function_table, &z_options, z_const, z_ret, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&z_const);
        zval_ptr_dtor(&z_ret);
#else
        zval z_const;
        zval z_ret;
        ZVAL_STRING(&z_const, "__construct");
        call_user_function(&HandlebarsOptions_ce_ptr->function_table, z_options, &z_const, &z_ret, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&z_const);
        zval_ptr_dtor(&z_ret);
#endif
    } while(0);

    intern = Z_HBS_OPTIONS_P(z_options);
    intern->vm = options->vm;
    intern->program = options->program;
    intern->inverse = options->inverse;

    if( options->name ) {
        zend_update_property_stringl(Z_OBJCE_P(z_options), z_options, ZEND_STRL("name"), options->name, talloc_array_length(options->name) - 1 TSRMLS_CC);
    }

    // @todo we should use closures
    if( options->program >= 0 ) {
        zend_update_property_long(Z_OBJCE_P(z_options), z_options, ZEND_STRL("fn"), options->program TSRMLS_CC);
    }
    if( options->inverse >= 0 ) {
        zend_update_property_long(Z_OBJCE_P(z_options), z_options, ZEND_STRL("inverse"), options->inverse TSRMLS_CC);
    }
}
/* }}} */

/* {{{ proto Handlebars\Options::__construct([array $props]) */
PHP_METHOD(HandlebarsOptions, __construct)
{
    zval * _this_zval;
    zval * props = NULL;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|z",
            &_this_zval, HandlebarsOptions_ce_ptr, &props) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(props)
    ZEND_PARSE_PARAMETERS_END();
#endif

    if( props && Z_TYPE_P(props) == IS_ARRAY ) {
        HashTable * ht = Z_ARRVAL_P(props);
#ifdef ZEND_ENGINE_3
        zend_string * key;
        zend_ulong index;
        zend_ulong idx = 0;
        zval * entry;

        ZEND_HASH_FOREACH_KEY_VAL(ht, index, key, entry) {
            zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZSTR_VAL(key), ZSTR_LEN(key), entry);
        } ZEND_HASH_FOREACH_END();
#else
        HashPosition data_pointer = NULL;
        zval ** data_entry = NULL;
        char * key;
        int key_len;
        long index;
        long idx = 0;

        zend_hash_internal_pointer_reset_ex(ht, &data_pointer);
        while( zend_hash_get_current_data_ex(ht, (void**) &data_entry, &data_pointer) == SUCCESS ) {
            if (zend_hash_get_current_key_ex(ht, &key, &key_len, &index, 0, &data_pointer) == HASH_KEY_IS_STRING) {
                zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, key, key_len - 1, *data_entry TSRMLS_CC);
            }
            zend_hash_move_forward_ex(ht, &data_pointer);
        }
#endif
    }
}
/* }}} Handlebars\Options::__construct */

static inline void php_handlebars_options_call(INTERNAL_FUNCTION_PARAMETERS, short program)
{
    zval * _this_zval;
    zval * z_context = NULL;
    zval * z_options = NULL;
    zval * z_entry;
    struct php_handlebars_options_obj * intern;
    struct handlebars_vm * vm;
    long programGuid;
    struct handlebars_value * context;
    struct handlebars_value * data = NULL;
    struct handlebars_value * block_params = NULL;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|zz",
            &_this_zval, HandlebarsOptions_ce_ptr, &z_context, &z_options) == FAILURE) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(z_context)
        Z_PARAM_ZVAL(z_options)
    ZEND_PARSE_PARAMETERS_END();
#endif

    intern = Z_HBS_OPTIONS_P(_this_zval);
    vm = intern->vm;

    if( !vm ) {
        // This was probably constructed in user land
        zval * z_fn;
        if( program ) {
            //z_fn = php5to7_zend_hash_find(Z_ARRVAL_P(z_options), ZEND_STRL("data"))
            z_fn = php5to7_zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("fn"), 0);
        } else {
            z_fn = php5to7_zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("inverse"), 0);
        }

        if( z_fn && Z_TYPE_P(z_fn) == IS_OBJECT ) {
#ifdef ZEND_ENGINE_3
            zval z_const;
            zval z_ret;
            zval * z_const_args = ecalloc(ZEND_NUM_ARGS(), sizeof(zval));
            if( ZEND_NUM_ARGS() >= 1 ) {
                //ZVAL_DUP(&z_const_args[0], z_context);
                z_const_args[0] = *z_context;
            }
            if( ZEND_NUM_ARGS() >= 2 ) {
                //ZVAL_DUP(&z_const_args[1], z_context);
                z_const_args[1] = *z_options;
            }
            ZVAL_NULL(&z_ret);
            ZVAL_STRING(&z_const, "__invoke");

            call_user_function(&Z_OBJCE_P(z_fn)->function_table, z_fn, &z_const, &z_ret, ZEND_NUM_ARGS(), z_const_args TSRMLS_CC);

            RETVAL_ZVAL(&z_ret, 1, 1);
            zval_ptr_dtor(&z_const);
#else
            zval * z_const;
            zval * z_ret;
            zval **z_const_args = emalloc(ZEND_NUM_ARGS() * sizeof(zval *));
            if( ZEND_NUM_ARGS() >= 1 ) {
                z_const_args[0] = z_context;
            }
            if( ZEND_NUM_ARGS() >= 2 ) {
                z_const_args[1] = z_options;
            }
            MAKE_STD_ZVAL(z_ret);
            ZVAL_STRING(z_const, "__invoke", 1);
            call_user_function(&Z_OBJCE_P(z_fn)->function_table, &z_fn, z_const, z_ret, ZEND_NUM_ARGS(), z_const_args TSRMLS_CC);
            efree(z_const_args);
            RETVAL_ZVAL(z_ret, 1, 0);
            zval_ptr_dtor(&z_ret);
            zval_ptr_dtor(&z_const);
#endif
        } else {
            zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "fn is not set", 0 TSRMLS_CC);
        }
        return;
    }

    if( program ) {
        programGuid = intern->program;
    } else {
        programGuid = intern->inverse;
    }

    if( programGuid < 0 ) {
        // nothing to do
        return;
    }

    // Context
    if( z_context ) {
        context = handlebars_value_from_zval(vm->ctx, z_context TSRMLS_CC);
    } else {
        context = handlebars_value_ctor(vm->ctx);
    }

    // Options
    if( z_options && Z_TYPE_P(z_options) == IS_ARRAY ) {
        if( NULL != (z_entry = php5to7_zend_hash_find(Z_ARRVAL_P(z_options), ZEND_STRL("data"))) ) {
            data = handlebars_value_from_zval(vm->ctx, z_entry TSRMLS_CC);
        }
        if( NULL != (z_entry = php5to7_zend_hash_find(Z_ARRVAL_P(z_options), ZEND_STRL("blockParams"))) ) {
            block_params = handlebars_value_from_zval(vm->ctx, z_entry TSRMLS_CC);
        }
        // @todo block params?
    }

    // Execute
    char * ret = handlebars_vm_execute_program_ex(vm, programGuid, context, data, block_params);
    PHP5TO7_RETVAL_STRINGL(ret, talloc_array_length(ret) - 1);
    talloc_free(ret);
}

PHP_METHOD(HandlebarsOptions, fn)
{
    php_handlebars_options_call(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_METHOD(HandlebarsOptions, inverse)
{
    php_handlebars_options_call(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_METHOD(HandlebarsOptions, offsetExists)
{
    zval * _this_zval;
    char * offset;
    strsize_t offset_len;
    zval * prop;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "Os",
            &_this_zval, HandlebarsOptions_ce_ptr, &offset, &offset_len) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STRING(offset, offset_len)
    ZEND_PARSE_PARAMETERS_END();
#endif

    prop = php5to7_zend_read_property2(Z_OBJCE_P(_this_zval), _this_zval, offset, offset_len, 1);
    RETURN_BOOL(prop != NULL);
}

PHP_METHOD(HandlebarsOptions, offsetGet)
{
    zval * _this_zval;
    char * offset;
    strsize_t offset_len;
    zval * prop;

#ifndef FAST_ZPP
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "Os",
            &_this_zval, HandlebarsOptions_ce_ptr, &offset, &offset_len) == FAILURE ) {
        return;
    }
#else
    _this_zval = getThis();
    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STRING(offset, offset_len)
    ZEND_PARSE_PARAMETERS_END();
#endif

    prop = php5to7_zend_read_property2(Z_OBJCE_P(_this_zval), _this_zval, offset, offset_len, 1);
    RETURN_ZVAL(prop, 1, 0);
}

PHP_METHOD(HandlebarsOptions, offsetSet)
{
    zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "offsetSet is not implemented", 0 TSRMLS_CC);
}

PHP_METHOD(HandlebarsOptions, offsetUnset)
{
    zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "offsetUnset is not implemented", 0 TSRMLS_CC);
}

/* {{{ Handlebars\Options methods */
static zend_function_entry HandlebarsOptions_methods[] = {
    PHP_ME(HandlebarsOptions, __construct, HandlebarsOptions_construct_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsOptions, fn, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsOptions, inverse, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsOptions, offsetExists, HandlebarsOptions_offsetExists_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsOptions, offsetGet, HandlebarsOptions_offsetExists_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsOptions, offsetSet, HandlebarsOptions_offsetSet_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsOptions, offsetUnset, HandlebarsOptions_offsetExists_args, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} Handlebars\Options methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_options)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;

    memcpy(&HandlebarsOptions_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
#ifdef ZEND_ENGINE_3
    HandlebarsOptions_obj_handlers.offset = XtOffsetOf(struct php_handlebars_options_obj, std);
    HandlebarsOptions_obj_handlers.free_obj = php_handlebars_options_obj_free;
#endif
    HandlebarsOptions_obj_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Options", HandlebarsOptions_methods);
    HandlebarsOptions_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    HandlebarsOptions_ce_ptr->create_object = php_handlebars_options_obj_create;
    zend_class_implements(HandlebarsOptions_ce_ptr TSRMLS_CC, 1, zend_ce_arrayaccess);

    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("name"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hash"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("fn"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("inverse"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("scope"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("data"), ZEND_ACC_PUBLIC TSRMLS_CC);

    // Used by handlebars.php
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("ids"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hashIds"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hashTypes"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hashContexts"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("types"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("contexts"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("args"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("partial"), ZEND_ACC_PUBLIC TSRMLS_CC);

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
