
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "main/php.h"

#include "handlebars.h"
#include "handlebars_private.h"
#include "handlebars_memory.h"
#include "handlebars_cache.h"
#include "handlebars_compiler.h"
#include "handlebars_helpers.h"
#include "handlebars_opcodes.h"
#include "handlebars_string.h"
#include "handlebars_value.h"
#include "handlebars_vm.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php5to7.h"
#include "php_handlebars.h"



/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsVM_ce_ptr;
static zend_object_handlers HandlebarsVM_obj_handlers;

struct php_handlebars_vm_obj {
#if PHP_MAJOR_VERSION < 7
    zend_object std;
#endif
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
static inline void php_handlebars_vm_obj_create_common(struct php_handlebars_vm_obj *obj TSRMLS_DC)
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
    php_handlebars_vm_obj_create_common(obj TSRMLS_CC);

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
    php_handlebars_vm_obj_create_common(obj TSRMLS_CC);

    return retval;
}
#endif
/* }}} */

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
