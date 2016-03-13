
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

#include "handlebars_helpers.h"
#include "handlebars_value.h"
#include "handlebars_vm.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsOptions_ce_ptr;
static zend_object_handlers HandlebarsOptions_obj_handlers;
static HashTable HandlebarsOptions_prop_handlers;
/* }}} Variables & Prototypes */

struct php_handlebars_options_obj {
#if PHP_MAJOR_VERSION < 7
    zend_object std;
#endif
    struct handlebars_options options;
    zend_object_handlers * std_hnd;
#if PHP_MAJOR_VERSION >= 7
    zend_object std;
#endif
};

struct hbs_prop_handlers {
    zend_object_read_property_t read_property;
    zend_object_has_property_t has_property;
};

#ifdef ZEND_ENGINE_3
#define READ_PROPERTY_ARGS zval *object, zval *member, int type, void **cache_slot, zval *rv
#define READ_PROPERTY_ARGS_PASSTHRU object, member, type, cache_slot, rv
#else
#define READ_PROPERTY_ARGS zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC
#define READ_PROPERTY_ARGS_PASSTHRU object, member, type, key TSRMLS_CC
#endif

static inline void register_prop_handler(const char * name, zend_object_read_property_t read_property)
{
    struct hbs_prop_handlers hnd;
    hnd.read_property = read_property;
    hnd.has_property = NULL;
    php5to7_zend_hash_update_mem(&HandlebarsOptions_prop_handlers, name, strlen(name), &hnd, sizeof(struct hbs_prop_handlers));
}

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
static inline void php_handlebars_options_obj_free_common(struct php_handlebars_options_obj * intern)
{
    handlebars_value_try_delref(intern->options.scope);
    handlebars_value_try_delref(intern->options.hash);
    handlebars_value_try_delref(intern->options.data);
}
#ifdef ZEND_ENGINE_3
static void php_handlebars_options_obj_free(zend_object * object TSRMLS_DC)
{
    struct php_handlebars_options_obj * payload = php_handlebars_options_fetch_object(object TSRMLS_CC);
    php_handlebars_options_obj_free_common(payload);
    zend_object_std_dtor((zend_object *)object TSRMLS_CC);
}
#else
static void php_handlebars_options_obj_free(void *object TSRMLS_DC)
{
    struct php_handlebars_options_obj * payload = (struct php_handlebars_options_obj *) object;
    php_handlebars_options_obj_free_common(payload);
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

    obj->std_hnd = zend_get_std_object_handlers();
    obj->options.program = -1;
    obj->options.inverse = -1;

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

    obj->std_hnd = zend_get_std_object_handlers();
    obj->options.program = -1;
    obj->options.inverse = -1;

    return retval;
}
#endif
/* }}} */

/* {{{ php_handlebars_options_ctor */
PHPAPI void php_handlebars_options_ctor(
        struct handlebars_options * options,
        zval * z_options TSRMLS_DC
) {
    struct php_handlebars_options_obj * intern;

    object_init_ex(z_options, HandlebarsOptions_ce_ptr);

    /*
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
    */

    intern = Z_HBS_OPTIONS_P(z_options);
    intern->options = *options;
    handlebars_value_try_addref(intern->options.scope);
    handlebars_value_try_addref(intern->options.hash);
    handlebars_value_try_addref(intern->options.data);
}
/* }}} */

/* {{{ Object handlers */
static zval * hbs_read_name(READ_PROPERTY_ARGS)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    if( intern->options.name ) {
        zend_update_property_stringl(Z_OBJCE_P(object), object, ZEND_STRL("name"), intern->options.name->val, intern->options.name->len TSRMLS_CC);
        intern->options.name = NULL;
    }
    return intern->std_hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
}
static zval * hbs_read_program(READ_PROPERTY_ARGS)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    if( intern->options.program >= 0 ) {
        zend_update_property_long(Z_OBJCE_P(object), object, ZEND_STRL("fn"), intern->options.program TSRMLS_CC);
        // @todo clear?
    }
    return intern->std_hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
}
static zval * hbs_read_inverse(READ_PROPERTY_ARGS)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    if( intern->options.inverse >= 0 ) {
        zend_update_property_long(Z_OBJCE_P(object), object, ZEND_STRL("inverse"), intern->options.inverse TSRMLS_CC);
        // @todo clear?
    }
    return intern->std_hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
}
static zval * hbs_read_scope(READ_PROPERTY_ARGS)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    if( intern->options.scope ) {
        zval * z_scope;
        PHP5TO7_MAKE_STD_ZVAL(z_scope);
        handlebars_value_to_zval(intern->options.scope, z_scope);
        zend_update_property(Z_OBJCE_P(object), object, ZEND_STRL("scope"), z_scope);
        php5to7_zval_ptr_dtor(z_scope);
        intern->options.scope = NULL;
    }
    return intern->std_hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
}
static zval * hbs_read_hash(READ_PROPERTY_ARGS)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    if( intern->options.hash ) {
        zval * z_hash;
        PHP5TO7_MAKE_STD_ZVAL(z_hash);
        handlebars_value_to_zval(intern->options.hash, z_hash);
        zend_update_property(Z_OBJCE_P(object), object, ZEND_STRL("hash"), z_hash);
        php5to7_zval_ptr_dtor(z_hash);
        intern->options.hash = NULL;
    }
    return intern->std_hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
}
static zval * hbs_read_data(READ_PROPERTY_ARGS)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    if( intern->options.data ) {
        zval * z_data;
        PHP5TO7_MAKE_STD_ZVAL(z_data);
        handlebars_value_to_zval(intern->options.data, z_data);
        zend_update_property(Z_OBJCE_P(object), object, ZEND_STRL("data"), z_data TSRMLS_CC);
        php5to7_zval_ptr_dtor(z_data);
        intern->options.data = NULL;
    }
    return intern->std_hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
}
#ifdef ZEND_ENGINE_3
static zval *php_handlebars_options_object_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv)
{
    zend_string *member_str = zval_get_string(member);
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    struct hbs_prop_handlers * hnd = zend_hash_find_ptr(&HandlebarsOptions_prop_handlers, member_str);
    if( hnd ) {
        return hnd->read_property(object, member, type, cache_slot, rv);
    } else {
        return intern->std_hnd->read_property(object, member, type, cache_slot, rv);
    }
}
static int php_handlebars_options_object_has_property(zval *object, zval *member, int check_empty, void **cache_slot)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    zval tmp;
    php_handlebars_options_object_read_property(object, member, 0, cache_slot, &tmp);
    return intern->std_hnd->has_property(object, member, check_empty, cache_slot);
}
#else
static zval *php_handlebars_options_object_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    struct hbs_prop_handlers * hnd;

	if (Z_TYPE_P(member) != IS_STRING) {
        return NULL;
	}

	if( zend_hash_find(&HandlebarsOptions_prop_handlers, Z_STRVAL_P(member), Z_STRLEN_P(member), (void **) &hnd) == SUCCESS ) {
        return hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
    } else {
        return intern->std_hnd->read_property(READ_PROPERTY_ARGS_PASSTHRU);
    }
}
static int php_handlebars_options_object_has_property(zval *object, zval *member, int has_set_exists, const zend_literal *key TSRMLS_DC)
{
    struct php_handlebars_options_obj * intern = Z_HBS_OPTIONS_P(object);
    zval * tmp = php_handlebars_options_object_read_property(object, member, 0, key TSRMLS_CC);
    return intern->std_hnd->has_property(object, member, has_set_exists, key TSRMLS_CC);
}
#endif
/* }}} Object handlers

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

    zend_update_property_null(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("fn") TSRMLS_CC);
    zend_update_property_null(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("inverse") TSRMLS_CC);

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
    jmp_buf buf;
    jmp_buf * prev;

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
    vm = intern->options.vm;

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
            MAKE_STD_ZVAL(z_const);
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
        programGuid = intern->options.program;
    } else {
        programGuid = intern->options.inverse;
    }

    if( programGuid < 0 ) {
        // nothing to do
        return;
    }

    // Context
    if( z_context ) {
        context = handlebars_value_from_zval(HBSCTX(vm), z_context TSRMLS_CC);
    } else {
        context = handlebars_value_ctor(HBSCTX(vm));
    }

    // Options
    if( z_options && Z_TYPE_P(z_options) == IS_ARRAY ) {
        if( NULL != (z_entry = php5to7_zend_hash_find(Z_ARRVAL_P(z_options), ZEND_STRL("data"))) ) {
            data = handlebars_value_from_zval(HBSCTX(vm), z_entry TSRMLS_CC);
        }
        if( NULL != (z_entry = php5to7_zend_hash_find(Z_ARRVAL_P(z_options), ZEND_STRL("blockParams"))) ) {
            block_params = handlebars_value_from_zval(HBSCTX(vm), z_entry TSRMLS_CC);
        }
        // @todo block params?
    }

    // Save jump buffer;
    prev = HBSCTX(vm)->jmp;
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, vm, &buf);

    // Execute
    char * ret = handlebars_vm_execute_program_ex(vm, programGuid, context, data, block_params);
    PHP5TO7_RETVAL_STRINGL(ret, talloc_array_length(ret) - 1);
    talloc_free(ret);

done:
    HBSCTX(vm)->jmp = prev;
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
    HandlebarsOptions_obj_handlers.has_property = php_handlebars_options_object_has_property;
    HandlebarsOptions_obj_handlers.read_property = php_handlebars_options_object_read_property;
    //HandlebarsOptions_obj_handlers.get_properties = php_handlebars_options_object_get_properties;
    HandlebarsOptions_obj_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Options", HandlebarsOptions_methods);
    HandlebarsOptions_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    HandlebarsOptions_ce_ptr->create_object = php_handlebars_options_obj_create;
    zend_class_implements(HandlebarsOptions_ce_ptr TSRMLS_CC, 1, zend_ce_arrayaccess);

    zend_hash_init(&HandlebarsOptions_prop_handlers, 0, NULL, NULL, 1);
    register_prop_handler("name", hbs_read_name);
    register_prop_handler("program", hbs_read_program);
    register_prop_handler("inverse", hbs_read_inverse);
    register_prop_handler("scope", hbs_read_scope);
    register_prop_handler("hash", hbs_read_hash);
    register_prop_handler("data", hbs_read_data);

    // Note: declaring these prevents dynamic initialization in PHP7
    /*
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("name"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("hash"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("fn"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("inverse"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("scope"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(HandlebarsOptions_ce_ptr, ZEND_STRL("data"), ZEND_ACC_PUBLIC TSRMLS_CC);
    */

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

/* {{{ PHP_MINIT_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(handlebars_options)
{
    zend_hash_destroy(&HandlebarsOptions_prop_handlers);

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
