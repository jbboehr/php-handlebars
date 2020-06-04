
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_string.h"
#include "Zend/zend_interfaces.h"
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#define XXH_PRIVATE_API
#define XXH_INLINE_ALL
#include "xxhash.h"
#pragma GCC diagnostic pop



/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsVM_ce_ptr;
static zend_object_handlers HandlebarsVM_obj_handlers;
static zend_string *INTERNED_CACHE_ID;

struct php_handlebars_vm_obj {
    struct handlebars_context * context;
    struct handlebars_value * helpers;
    struct handlebars_value * partials;
    zend_object std;
};

extern zend_string *HANDLEBARS_INTERNED_STR_LOGGER;
extern zend_string *HANDLEBARS_INTERNED_STR_HELPERS;
extern zend_string *HANDLEBARS_INTERNED_STR_PARTIALS;
extern zend_string *HANDLEBARS_INTERNED_STR_DECORATORS;
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
    zval * logger = zend_read_property_ex(HandlebarsBaseImpl_ce_ptr, z_vm, HANDLEBARS_INTERNED_STR_LOGGER, 1, NULL);
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
        zval z_args[2] = {0};
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
HBS_ATTR_NONNULL_ALL
void php_handlebars_fetch_known_helpers(struct handlebars_compiler * compiler, struct handlebars_value * helpers)
{
    const char ** orig = compiler->known_helpers; //handlebars_builtins_names();
    long num = 0;
    long idx = 0;
    char ** known_helpers;
    const char ** ptr2;
    struct handlebars_value_iterator it;

    // Count the number of builtins
    for( ptr2 = orig ; *ptr2 ; ++ptr2 ) {
        num++;
    }

    // Count the number of helpers
    // this can be wrong?
    // num += handlebars_value_count(helpers);
    handlebars_value_iterator_init(&it, helpers);
    for( ; it.current != NULL; it.next(&it) ) {
        num++;
    }

    // Alloc the array
    known_helpers = handlebars_talloc_array(compiler, char *, num + 1);

    // Copy in the builtins
    for( ptr2 = orig ; *ptr2 ; ++ptr2 ) {
        known_helpers[idx++] = handlebars_talloc_strdup(known_helpers, *ptr2);
    }

    // Copy in the helpers
    handlebars_value_iterator_init(&it, helpers);
    for( ; it.current != NULL; it.next(&it) ) {
        known_helpers[idx++] = handlebars_talloc_strndup(known_helpers, it.key->val, it.key->len);
    }

    known_helpers[idx++] = 0;
    compiler->known_helpers = (const char **) known_helpers;
}
/* }}} php_handlebars_fetch_known_helpers */

HBS_ATTR_NONNULL_ALL
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
    zend_update_property_ex(Z_OBJCE_P(_this_zval), _this_zval, HANDLEBARS_INTERNED_STR_HELPERS, helpers);
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

    RETURN_ZVAL(_this_zval, 1, 0);
}

HBS_ATTR_NONNULL_ALL
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
    zend_update_property_ex(Z_OBJCE_P(_this_zval), _this_zval, HANDLEBARS_INTERNED_STR_PARTIALS, partials);
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

    RETURN_ZVAL(_this_zval, 1, 0);
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
        zval * helpers = zend_hash_find(ht, HANDLEBARS_INTERNED_STR_HELPERS);
        zval * partials = zend_hash_find(ht, HANDLEBARS_INTERNED_STR_PARTIALS);
        zval * decorators = zend_hash_find(ht, HANDLEBARS_INTERNED_STR_DECORATORS);
        zval * logger = zend_hash_find(ht, HANDLEBARS_INTERNED_STR_LOGGER);
        if( helpers ) {
            php_handlebars_vm_set_helpers(_this_zval, helpers);
        }
        if( partials ) {
            php_handlebars_vm_set_partials(_this_zval, partials);
        }
        if( decorators ) {
            zend_update_property_ex(Z_OBJCE_P(_this_zval), _this_zval, HANDLEBARS_INTERNED_STR_DECORATORS, decorators);
        }
        if( logger ) {
            // @todo check type
            zend_update_property_ex(Z_OBJCE_P(_this_zval), _this_zval, HANDLEBARS_INTERNED_STR_LOGGER, logger);
        }
    }
}

HBS_ATTR_NONNULL_ALL HBS_ATTR_RETURNS_NONNULL
static inline void *make_mctx(struct php_handlebars_vm_obj *intern) {
    void * rv;
    if( HANDLEBARS_G(pool_size) > 0 ) {
        rv = talloc_pool(intern->context, HANDLEBARS_G(pool_size));
    } else {
        rv = talloc_new(intern->context);
    }
    PHP_HBS_ASSERT(rv);
    return rv;
}

HBS_ATTR_NONNULL(1, 3, 4)
static struct handlebars_module * compile(
    struct handlebars_context *ctx,
    struct handlebars_vm *vm,
    struct handlebars_string *tmpl,
    zval *z_options
) {
    jmp_buf buf;
    struct handlebars_parser * parser;
    struct handlebars_compiler * compiler;
    struct handlebars_module * module = NULL;

    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
    parser = handlebars_parser_ctor(ctx);
    compiler = handlebars_compiler_ctor(ctx);

    // Set compiler options
    php_handlebars_process_options_zval(compiler, vm, z_options);
    if( vm && vm->helpers ) {
       php_handlebars_fetch_known_helpers(compiler, vm->helpers);
    }

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
    php_handlebars_try(HandlebarsCompileException_ce_ptr, parser, &buf);
    handlebars_parse(parser);

    // Compile
    php_handlebars_try(HandlebarsCompileException_ce_ptr, compiler, &buf);
    handlebars_compiler_compile(compiler, parser->program);

    module = handlebars_program_serialize(HBSCTX(ctx), compiler->program);
    module->flags = compiler->flags; // @todo is this correct?

done:
    return module;
}

#define HASH_LEN 8
typedef XXH64_hash_t hash_type;

static inline void hash_pack(hash_type val, char *out) {
    out[0] = (int)((val >> 56) & 0xFF);
    out[1] = (int)((val >> 48) & 0xFF) ;
    out[2] = (int)((val >> 40) & 0XFF);
    out[3] = (int)((val >> 32) & 0xFF);
    out[4] = (int)((val >> 24) & 0xFF);
    out[5] = (int)((val >> 16) & 0xFF) ;
    out[6] = (int)((val >> 8) & 0XFF);
    out[7] = (int)((val & 0XFF));
}

static void hash_bin2hex(const unsigned char *in, char *out) {
    static char hexconvtab[] = "0123456789abcdef";
	size_t i, j;

	for (i = j = 0; i < HASH_LEN; i++) {
		out[j++] = hexconvtab[in[i] >> 4];
		out[j++] = hexconvtab[in[i] & 15];
	}
	out[j] = '\0';
}

static hash_type hash_buf(unsigned char * buf, size_t len) {
    XXH3_state_t state;
    XXH3_64bits_reset(&state);
    XXH3_64bits_update(&state, buf, len);
    return XXH3_64bits_digest(&state);
}

HBS_ATTR_NONNULL_ALL
static inline struct handlebars_module * verify_and_load_module(
    struct handlebars_context * ctx,
    zend_string *buf
) {
    if( UNEXPECTED(ZSTR_LEN(buf) <= HASH_LEN) ) {
        zend_throw_exception(HandlebarsInvalidBinaryStringException_ce_ptr, "Failed to validate precompiled template: buffer not long enough", 0);
        return NULL;
    }

    size_t size = ZSTR_LEN(buf) - HASH_LEN;
    struct handlebars_module *UNSAFE_module = (struct handlebars_module *) (ZSTR_VAL(buf) + HASH_LEN);

    hash_type hash = hash_buf((unsigned char *) UNSAFE_module, size);
    char hash_arr[HASH_LEN];
    hash_pack(hash, hash_arr);

    if (0 != memcmp(hash_arr, ZSTR_VAL(buf), HASH_LEN)) {
        char actual_hex[HASH_LEN * 2 + 1] = {0};
        char expected_hex[HASH_LEN * 2 + 1] = {0};
        hash_bin2hex((unsigned char *) hash_arr, actual_hex);
        hash_bin2hex((unsigned char *) ZSTR_VAL(buf), expected_hex);
        zend_throw_exception_ex(HandlebarsInvalidBinaryStringException_ce_ptr, 0,
            "Failed to validate precompiled template: template hash was %s, expected %s",
            actual_hex,
            expected_hex);
        return NULL;
    } else if (UNSAFE_module->size != size) {
        zend_throw_exception_ex(HandlebarsInvalidBinaryStringException_ce_ptr, 0,
            "Failed to validate precompiled template: template data segment was %zu bytes, expected %zu",
            size,
            UNSAFE_module->size);
        return NULL;
    } else if (UNSAFE_module->version != handlebars_version()) {
        zend_throw_exception_ex(HandlebarsInvalidBinaryStringException_ce_ptr, 0,
            "Failed to validate precompiled template: template was compiled with handlebars %d, current version is %d",
            UNSAFE_module->version,
            handlebars_version());
        return NULL;
    }

    struct handlebars_module * module = handlebars_talloc_zero(ctx, struct handlebars_module);
    module = handlebars_talloc_realloc_size(ctx, module, size);
    memcpy(module, ZSTR_VAL(buf) + HASH_LEN, size);

    handlebars_module_patch_pointers(module);

    return module;
}

PHP_METHOD(HandlebarsVM, compile)
{
    zend_string * zstr_tmpl;
    zval * z_options = NULL;

    zval * _this_zval = getThis();
    PHP_HBS_ASSERT(_this_zval);
    struct php_handlebars_vm_obj *intern = Z_HANDLEBARS_VM_P(_this_zval);
    void *mctx = make_mctx(intern);
    struct handlebars_context *ctx = handlebars_context_ctor_ex(mctx);
    struct handlebars_vm * vm = handlebars_vm_ctor(ctx);

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(zstr_tmpl)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(z_options)
    ZEND_PARSE_PARAMETERS_END();

    struct handlebars_string * tmpl = handlebars_string_ctor(HBSCTX(ctx), ZSTR_VAL(zstr_tmpl), ZSTR_LEN(zstr_tmpl));

    struct handlebars_module * module = compile(ctx, NULL, tmpl, z_options);
    if (!module) {
        goto done;
    }

#if HANDLEBARS_VERSION_INT >= 701
    handlebars_module_normalize_pointers(module, (void *) 1);
#endif

    hash_type hash = hash_buf((unsigned char *) module, module->size);

    size_t len = HASH_LEN + module->size;
	zend_string *res = zend_string_alloc(len, 0);
    hash_pack(hash, ZSTR_VAL(res));
	memcpy(ZSTR_VAL(res) + HASH_LEN, module, module->size);
	ZSTR_VAL(res)[len] = '\0';

    RETVAL_STR(res);

done:
    handlebars_vm_dtor(vm);
    handlebars_talloc_free(mctx);
}

enum input_type {
    input_type_string = 0,
    input_type_filename = 1,
    input_type_binary = 2
};

HBS_ATTR_NONNULL_ALL
static void render(INTERNAL_FUNCTION_PARAMETERS, enum input_type type)
{
    zend_string * zstr_input;
    zval * z_context = NULL;
    zval * z_options = NULL;
    struct handlebars_module * module = NULL;
    struct handlebars_value * context;
    jmp_buf buf;
    bool from_cache = false;
    struct handlebars_string * cache_id;
    struct handlebars_string * input;
    zval * tmp = NULL;

    zval *_this_zval = getThis();
    struct php_handlebars_vm_obj *intern = Z_HANDLEBARS_VM_P(_this_zval);
    void *mctx = make_mctx(intern);
    struct handlebars_context *ctx = handlebars_context_ctor_ex(mctx);
    struct handlebars_vm * vm = handlebars_vm_ctor(ctx);
    struct handlebars_cache * cache = HANDLEBARS_G(cache);

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STR(zstr_input)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(z_context)
        Z_PARAM_ZVAL(z_options)
    ZEND_PARSE_PARAMETERS_END();

    // Dereference zval
    if( z_context && Z_TYPE_P(z_context) == IS_REFERENCE ) {
        ZVAL_DEREF(z_context);
    }

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

    input = handlebars_string_ctor(HBSCTX(vm), ZSTR_VAL(zstr_input), ZSTR_LEN(zstr_input));

    // Check cache id
    if( NULL != z_options
            && NULL != (tmp = zend_hash_find(Z_ARRVAL_P(z_options), INTERNED_CACHE_ID))
            && Z_TYPE_P(tmp) == IS_STRING ) {
        cache_id = handlebars_string_ctor(HBSCTX(ctx), Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
    } else if (type == input_type_binary) {
        cache_id = NULL;
    } else {
        cache_id = input;
    }

    // Lookup cache entry
    if( cache && cache_id && (module = handlebars_cache_find(cache, cache_id)) ) {
        // If we're a file, and stat is enable, check if it's too old
        if( type == input_type_filename && HANDLEBARS_G(cache_stat) ) {
            zval zstat;
            ZVAL_LONG(&zstat, 0);
            php_stat(ZSTR_VAL(zstr_input), ZSTR_LEN(zstr_input), FS_MTIME, &zstat);
            if( Z_LVAL(zstat) > module->ts ) { // possibly not portable
                cache->release(cache, cache_id, module);
                from_cache = false;
                module = NULL;
            }
        }

        // Use cached
        from_cache = true;
    }

    if( !module ) {
        struct handlebars_string * tmpl;

        switch (type) {
            case input_type_string:
                tmpl = input;
                break;

            case input_type_filename: {
                php_stream *stream;

                stream = php_stream_open_wrapper_ex(ZSTR_VAL(input), "rb", REPORT_ERRORS, NULL, NULL);
                if( !stream ) {
                    zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Failed to read input template file", 0);
                    goto done;
                }

                zend_string *contents = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
                php_stream_close(stream);
                if( contents != NULL ) {
                    tmpl = handlebars_string_ctor(HBSCTX(vm), contents->val, contents->len);
                } else {
                    zend_throw_exception(HandlebarsRuntimeException_ce_ptr, "Failed to read input template file", 0);
                    goto done;
                }

                break;
            }

            case input_type_binary: {
                // Verify and load
                module = verify_and_load_module(ctx, zstr_input);
                if (!module) {
                    goto done;
                }
                goto skip_compile;
                break;
            }

            default: assert(0); break; // LCOV_EXCL_LINE
        }

        module = compile(ctx, vm, tmpl, z_options);
        if (!module) {
            goto done;
        }

skip_compile:

        // Save cache entry
        if( cache && cache_id ) {
            handlebars_cache_add(cache, cache_id, module);
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
        cache->release(cache, cache_id, module);
    }
    handlebars_vm_dtor(vm);
    handlebars_talloc_free(mctx);
}

PHP_METHOD(HandlebarsVM, render)
{
    render(INTERNAL_FUNCTION_PARAM_PASSTHRU, input_type_string);
}

PHP_METHOD(HandlebarsVM, renderFile)
{
    render(INTERNAL_FUNCTION_PARAM_PASSTHRU, input_type_filename);
}

PHP_METHOD(HandlebarsVM, renderFromBinaryString)
{
    render(INTERNAL_FUNCTION_PARAM_PASSTHRU, input_type_binary);
}

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsVM_construct_args, ZEND_SEND_BY_VAL, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsVM, compile, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsVM, renderFromBinaryString, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, binaryString, IS_STRING, 0)
    ZEND_ARG_INFO(0, context)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsVM methods */
static zend_function_entry HandlebarsVM_methods[] = {
    PHP_ME(HandlebarsVM, __construct, HandlebarsVM_construct_args, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, setHelpers, arginfo_HandlebarsImpl_setHelpers, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, setPartials, arginfo_HandlebarsImpl_setPartials, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, compile, arginfo_HandlebarsVM_compile, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, render, arginfo_HandlebarsImpl_render, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, renderFile, arginfo_HandlebarsImpl_renderFile, ZEND_ACC_PUBLIC)
    PHP_ME(HandlebarsVM, renderFromBinaryString, arginfo_HandlebarsVM_renderFromBinaryString, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} HandlebarsVM methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_vm)
{
    zend_class_entry ce;

    INTERNED_CACHE_ID = zend_new_interned_string(zend_string_init(ZEND_STRL("cacheId"), 1));

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
