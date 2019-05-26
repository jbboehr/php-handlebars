
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "main/php.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/php_filestat.h"

#include "handlebars.h"
#include "handlebars_private.h"
#include "handlebars_memory.h"

#include "handlebars_cache.h"
#include "handlebars_compiler.h"
#include "handlebars_helpers.h"
#include "handlebars_opcodes.h"
#include "handlebars_opcode_serializer.h"
#include "handlebars_string.h"
#include "handlebars_value.h"
#include "handlebars_vm.h"

#define BOOLEAN HBS_BOOLEAN
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php_handlebars.h"



/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsVM_ce_ptr;
static zend_object_handlers HandlebarsVM_obj_handlers;

struct php_handlebars_vm_obj {
    struct handlebars_context * context;
    struct handlebars_value * helpers;
    struct handlebars_value * partials;
    zend_object std;
};
/* }}} Variables & Prototypes */

/* {{{ Z_HANDLEBARS_VM_P */
static inline struct php_handlebars_vm_obj * php_handlebars_vm_fetch_object(zend_object *obj) {
    return (struct php_handlebars_vm_obj *)((char*)(obj) - XtOffsetOf(struct php_handlebars_vm_obj, std));
}
#define Z_HANDLEBARS_VM_P(zv) php_handlebars_vm_fetch_object(Z_OBJ_P((zv)))
/* }}} */

/* {{{ php_handlebars_vm_obj_free */
static void php_handlebars_vm_obj_free(zend_object * object)
{
    struct php_handlebars_vm_obj * obj = php_handlebars_vm_fetch_object(object);

    if( obj->helpers ) {
        handlebars_value_dtor(obj->helpers);
    }
    if( obj->partials ) {
        handlebars_value_dtor(obj->partials);
    }
    handlebars_context_dtor(obj->context);

    zend_object_std_dtor((zend_object *)object);
}
/* }}} */

/* php_handlebars_log */
static void php_handlebars_log(
        int argc,
        struct handlebars_value * argv[],
        struct handlebars_options * options
) {
    zval * z_vm = (zval *) options->vm->log_ctx;
    zval * logger = zend_read_property(HandlebarsBaseImpl_ce_ptr, z_vm, ZEND_STRL("logger"), 1, NULL);
    char * message;
    size_t message_len;
    int i;

    // Generate message
    message = handlebars_talloc_strdup(HANDLEBARS_G(root), "");
    for (i = 0; i < argc; i++) {
        char *tmp = handlebars_value_dump(argv[i], 0);
        message = handlebars_talloc_asprintf_append_buffer(message, "%s ", tmp);
        handlebars_talloc_free(tmp);
    }
    message_len = talloc_array_length(message) - 1;

    if( logger && Z_TYPE_P(logger) == IS_OBJECT ) {
        // @todo Look up log level
        struct handlebars_value * level = options->hash ? handlebars_value_map_str_find(options->hash, HBS_STRL("level")) : NULL;
        const char * level_str = level && level->type == HANDLEBARS_VALUE_TYPE_STRING ? level->v.string->val : "info";

        zval z_fn = {0};
        zval z_ret = {0};
        zval z_args[2] = {{0}, {0}};
        ZVAL_STRING(&z_fn, level_str);
        ZVAL_STRINGL(&z_args[0], message, message_len);
        array_init(&z_args[1]);
        call_user_function(&Z_OBJCE_P(logger)->function_table, logger, &z_fn, &z_ret, 2, z_args);
        zval_ptr_dtor(&z_args[1]);
        zval_ptr_dtor(&z_args[0]);
        zval_ptr_dtor(&z_fn);
        zval_ptr_dtor(&z_ret);
    } else {
        _php_error_log_ex(4, message, message_len, NULL, NULL);
    }

    handlebars_talloc_free(message);
}
/* */

/* {{{ php_handlebars_vm_obj_create */
static inline void php_handlebars_vm_obj_create_common(struct php_handlebars_vm_obj *obj)
{
    obj->context = handlebars_context_ctor_ex(HANDLEBARS_G(root));
    obj->helpers = handlebars_value_ctor(obj->context);
    handlebars_value_map_init(obj->helpers);
    obj->partials = handlebars_value_ctor(obj->context);
    handlebars_value_map_init(obj->partials);
}
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
/* }}} */

/* {{{ php_handlebars_fetch_known_helpers */
void php_handlebars_fetch_known_helpers(struct handlebars_compiler * compiler, zval * helpers)
{
    HashTable * data_hash = NULL;
    long num;
    long idx = 0;
    char ** known_helpers;

    if( Z_TYPE_P(helpers) == IS_ARRAY ) {
        data_hash = Z_ARRVAL_P(helpers);
    } else if( Z_TYPE_P(helpers) == IS_OBJECT && Z_OBJ_HT_P(helpers)->get_properties != NULL ) {
        data_hash = Z_OBJ_HT_P(helpers)->get_properties(helpers);
    } else {
        return;
    }

    // @todo merge with existing helpers?

    num = zend_hash_num_elements(data_hash);
    known_helpers = handlebars_talloc_array(compiler, char *, num + 1);

    zend_string *key;
    zend_ulong index;
    ZEND_HASH_FOREACH_KEY(data_hash, index, key) {
        if( key ) {
            known_helpers[idx++] = handlebars_talloc_strndup(known_helpers, ZSTR_VAL(key), ZSTR_LEN(key));
        } else {
            (void) index;
        }
    } ZEND_HASH_FOREACH_END();

    known_helpers[idx++] = 0;
    compiler->known_helpers = (const char **) known_helpers;
}
/* }}} php_handlebars_fetch_known_helpers */

static void php_handlebars_vm_set_helpers(zval * _this_zval, zval * helpers)
{
    jmp_buf buf;
    struct php_handlebars_vm_obj * intern = Z_HANDLEBARS_VM_P(_this_zval);
    struct handlebars_context * context = intern->context;
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, context, &buf);
    if( intern->helpers ) {
        handlebars_value_dtor(intern->helpers);
    }
    intern->helpers = handlebars_value_from_zval(HBSCTX(context), helpers);
    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("helpers"), helpers);
done:
    context->e->jmp = NULL;
}
PHP_METHOD(HandlebarsVM, setHelpers)
{
    zval * _this_zval = getThis();
    zval * helpers;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(helpers, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();

    php_handlebars_vm_set_helpers(_this_zval, helpers);
}

static void php_handlebars_vm_set_partials(zval * _this_zval, zval * partials)
{
    jmp_buf buf;
    struct php_handlebars_vm_obj * intern = Z_HANDLEBARS_VM_P(_this_zval);
    struct handlebars_context * context = intern->context;
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, context, &buf);
    if( intern->partials ) {
        handlebars_value_dtor(intern->partials);
    }
    intern->partials = handlebars_value_from_zval(HBSCTX(context), partials);
    zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("partials"), partials);
done:
    context->e->jmp = NULL;
}

PHP_METHOD(HandlebarsVM, setPartials)
{
    zval * _this_zval = getThis();
    zval * partials;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_OBJECT_OF_CLASS(partials, HandlebarsRegistry_ce_ptr)
    ZEND_PARSE_PARAMETERS_END();

    php_handlebars_vm_set_partials(_this_zval, partials);
}

PHP_METHOD(HandlebarsVM, __construct)
{
    zval * _this_zval = getThis();
    zval * z_options = NULL;

    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_ARRAY(z_options)
    ZEND_PARSE_PARAMETERS_END();

    if( z_options && Z_TYPE_P(z_options) == IS_ARRAY ) {
        HashTable * ht = Z_ARRVAL_P(z_options);
        zval * helpers = zend_hash_str_find(ht, ZEND_STRL("helpers"));
        zval * partials = zend_hash_str_find(ht, ZEND_STRL("partials"));
        zval * logger = zend_hash_str_find(ht, ZEND_STRL("logger"));
        if( helpers ) {
            php_handlebars_vm_set_helpers(_this_zval, helpers);
        }
        if( partials ) {
            php_handlebars_vm_set_partials(_this_zval, partials);
        }
        if( logger ) {
            // @todo check type
            zend_update_property(Z_OBJCE_P(_this_zval), _this_zval, ZEND_STRL("logger"), logger);
        }
    }
}

PHP_METHOD(HandlebarsVM, render)
{
    zval * _this_zval = getThis();
    zend_string * tmpl_str;
    zval * z_context = NULL;
    zval * z_options = NULL;
    TALLOC_CTX * mctx;
    struct handlebars_cache * cache = NULL;
    struct handlebars_module * module = NULL;
    struct handlebars_context * ctx = NULL;
    struct handlebars_parser * parser;
    struct handlebars_compiler * compiler;
    struct handlebars_value * context;
    jmp_buf buf;
    bool from_cache = false;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STR(tmpl_str)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(z_context)
        Z_PARAM_ZVAL(z_options)
    ZEND_PARSE_PARAMETERS_END();

    // Dereference zval
    if( z_context && Z_TYPE_P(z_context) == IS_REFERENCE ) {
        ZVAL_DEREF(z_context);
    }

    struct php_handlebars_vm_obj * intern = Z_HANDLEBARS_VM_P(_this_zval);
    if( HANDLEBARS_G(pool_size) > 0 ) {
        mctx = talloc_pool(intern->context, HANDLEBARS_G(pool_size));
    } else {
        mctx = talloc_new(intern->context);
    }
    ctx = handlebars_context_ctor_ex(mctx);

    struct handlebars_vm * vm = handlebars_vm_ctor(ctx);
    cache = HANDLEBARS_G(cache);

    vm->cache = cache;
    if( intern->helpers ) {
        vm->helpers = intern->helpers;
        vm->helpers->ctx = ctx;
    }
    if( intern->partials ) {
        vm->partials = intern->partials;
        vm->partials->ctx = ctx;
    }
    vm->log_func = &php_handlebars_log;
    vm->log_ctx = _this_zval;

    struct handlebars_string * tmpl = handlebars_string_ctor(HBSCTX(vm), ZSTR_VAL(tmpl_str), ZSTR_LEN(tmpl_str));

    // Lookup cache entry
    if( cache && (module = handlebars_cache_find(cache, tmpl)) ) {
        // Use cached
        from_cache = true;
    } else {
        php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
        parser = handlebars_parser_ctor(ctx);
        compiler = handlebars_compiler_ctor(ctx);

        // Set compiler options
        php_handlebars_process_options_zval(compiler, vm, z_options);
        /*if( z_helpers ) {
            php_handlebars_fetch_known_helpers(compiler, z_helpers);
        }*/

        // Preprocess template
#if defined(HANDLEBARS_VERSION_INT) && HANDLEBARS_VERSION_INT >= 604
        if( compiler->flags & handlebars_compiler_flag_compat ) {
            parser->tmpl = handlebars_preprocess_delimiters(HBSCTX(ctx), tmpl, NULL, NULL);
        } else {
            parser->tmpl = tmpl;
        }
#else
        parser->tmpl = tmpl;
#endif

        // Parse
        php_handlebars_try(HandlebarsParseException_ce_ptr, parser, &buf);
        handlebars_parse(parser);

        // Compile
        php_handlebars_try(HandlebarsCompileException_ce_ptr, compiler, &buf);
        handlebars_compiler_compile(compiler, parser->program);

        // Serialize
        module = handlebars_program_serialize(HBSCTX(vm), compiler->program);
        module->flags = compiler->flags; // @todo is this correct?

        // Save cache entry
        if( cache ) {
            handlebars_cache_add(cache, tmpl, module);
        }
    }

    // Make context
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, vm, &buf);
    if( z_context ) {
        context = handlebars_value_from_zval(HBSCTX(vm), z_context);
    } else {
        context = handlebars_value_ctor(HBSCTX(vm));
    }

    // Execute
    vm->flags = module->flags;
    handlebars_vm_execute(vm, module, context);

    if( vm->buffer && !EG(exception) ) {
        RETVAL_STRINGL(vm->buffer->val, vm->buffer->len);
    }

done:
    if( intern->helpers ) {
        intern->helpers->ctx = NULL;
    }
    if( intern->partials ) {
        intern->partials->ctx = NULL;
    }
    if( from_cache ) {
        cache->release(cache, tmpl, module);
    }
    handlebars_vm_dtor(vm);
    handlebars_talloc_free(mctx);
}

PHP_METHOD(HandlebarsVM, renderFile)
{
    zval * _this_zval = getThis();
    zend_string * filename_str;
    zval * z_context = NULL;
    zval * z_options = NULL;
    void * mctx = NULL;
    struct handlebars_cache * cache = NULL;
    struct handlebars_module * module = NULL;
    struct handlebars_context * ctx = NULL;
    struct handlebars_parser * parser;
    struct handlebars_compiler * compiler;
    struct handlebars_value * context;
    jmp_buf buf;
    bool from_cache = false;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STR(filename_str)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(z_context)
        Z_PARAM_ZVAL(z_options)
    ZEND_PARSE_PARAMETERS_END();

    // Dereference zval
    if( z_context && Z_TYPE_P(z_context) == IS_REFERENCE ) {
        ZVAL_DEREF(z_context);
    }

    struct php_handlebars_vm_obj * intern = Z_HANDLEBARS_VM_P(_this_zval);
    if( HANDLEBARS_G(pool_size) > 0 ) {
        mctx = talloc_pool(intern->context, HANDLEBARS_G(pool_size));
    } else {
        mctx = talloc_new(intern->context);
    }
    ctx = handlebars_context_ctor_ex(mctx);

    struct handlebars_vm * vm = handlebars_vm_ctor(ctx);
    cache = HANDLEBARS_G(cache);

    vm->cache = cache;
    if( intern->helpers ) {
        vm->helpers = intern->helpers;
        vm->helpers->ctx = ctx;
    }
    if( intern->partials ) {
        vm->partials = intern->partials;
        vm->partials->ctx = ctx;
    }
    vm->log_func = &php_handlebars_log;
    vm->log_ctx = _this_zval;

    struct handlebars_string * filename = handlebars_string_ctor(HBSCTX(vm), ZSTR_VAL(filename_str), ZSTR_LEN(filename_str));

    // Lookup cache entry
    if( cache && (module = handlebars_cache_find(cache, filename)) ) {
        // Check if too old
        if( HANDLEBARS_G(cache_stat) ) {
            zval zstat;
            ZVAL_LONG(&zstat, 0);
            php_stat(ZSTR_VAL(filename_str), ZSTR_LEN(filename), FS_MTIME, &zstat);
            if( Z_LVAL(zstat) > module->ts ) { // possibly not portable
                cache->release(cache, filename, module);
                from_cache = false;
                module = NULL;
            }
        }

        // Use cached
        from_cache = true;
    }

    if( !module ) {
        // Read file
        php_stream *stream;
        struct handlebars_string * tmpl;

        stream = php_stream_open_wrapper_ex(ZSTR_VAL(filename_str), "rb", REPORT_ERRORS, NULL, NULL);
        if( !stream ) {
            RETVAL_FALSE;
            goto done;
        }

        zend_string *contents = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
        php_stream_close(stream);
        if( contents != NULL) {
            tmpl = handlebars_string_ctor(HBSCTX(vm), contents->val, contents->len);
        } else {
            RETVAL_FALSE;
            goto done;
        }

        //ctx = handlebars_context_ctor_ex(HANDLEBARS_G(root));
        php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
        parser = handlebars_parser_ctor(ctx);
        compiler = handlebars_compiler_ctor(ctx);

        // Set compiler options
        php_handlebars_process_options_zval(compiler, vm, z_options);
        /*if( z_helpers ) {
            php_handlebars_fetch_known_helpers(compiler, z_helpers);
        }*/

        // Preprocess template
#if defined(HANDLEBARS_VERSION_INT) && HANDLEBARS_VERSION_INT >= 604
        if( compiler->flags & handlebars_compiler_flag_compat ) {
            parser->tmpl = handlebars_preprocess_delimiters(HBSCTX(ctx), tmpl, NULL, NULL);
        } else {
            parser->tmpl = tmpl;
        }
#else
        parser->tmpl = tmpl;
#endif

        // Parse
        php_handlebars_try(HandlebarsParseException_ce_ptr, parser, &buf);
        handlebars_parse(parser);

        // Compile
        php_handlebars_try(HandlebarsCompileException_ce_ptr, compiler, &buf);
        handlebars_compiler_compile(compiler, parser->program);

        module = handlebars_program_serialize(HBSCTX(vm), compiler->program);
        module->flags = compiler->flags; // @todo is this correct?

        // Save cache entry
        if( cache ) {
            handlebars_cache_add(cache, filename, module);
            //handlebars_cache_add(cache, tmpl, program);
        }
    }

    // Make context
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, vm, &buf);
    context = handlebars_value_from_zval(HBSCTX(vm), z_context);

    // Execute
    vm->flags = module->flags;
    handlebars_vm_execute(vm, module, context);

    if( vm->buffer && !EG(exception) ) {
        RETVAL_STRINGL(vm->buffer->val, vm->buffer->len);
    }

done:
    if( intern->helpers ) {
        intern->helpers->ctx = NULL;
    }
    if( intern->partials ) {
        intern->partials->ctx = NULL;
    }
    if( from_cache ) {
        cache->release(cache, filename, module);
    }
    handlebars_vm_dtor(vm);
    handlebars_talloc_free(mctx);
}

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_construct_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()
/* }}} Argument Info */


/* {{{ HandlebarsVM methods */
static zend_function_entry HandlebarsVM_methods[] = {
    PHP_ME(HandlebarsVM, __construct, HandlebarsVM_construct_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, setHelpers, HandlebarsImpl_setHelpers_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, setPartials, HandlebarsImpl_setPartials_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, render, HandlebarsImpl_render_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, renderFile, HandlebarsImpl_renderFile_args, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} HandlebarsVM methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_vm)
{
    zend_class_entry ce;

    memcpy(&HandlebarsVM_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    HandlebarsVM_obj_handlers.offset = XtOffsetOf(struct php_handlebars_vm_obj, std);
    HandlebarsVM_obj_handlers.free_obj = php_handlebars_vm_obj_free;
    HandlebarsVM_obj_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "Handlebars\\VM", HandlebarsVM_methods);
    HandlebarsVM_ce_ptr = zend_register_internal_class_ex(&ce, HandlebarsBaseImpl_ce_ptr);
    HandlebarsVM_ce_ptr ->create_object = php_handlebars_vm_obj_create;

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
