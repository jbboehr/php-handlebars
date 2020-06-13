
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_string.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_types.h"

#include "main/php.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/php_filestat.h"

#define HANDLEBARS_HELPERS_PRIVATE

#include "handlebars.h"
#include "handlebars_memory.h"

#include "handlebars_cache.h"
#include "handlebars_compiler.h"
#include "handlebars_delimiters.h"
#include "handlebars_helpers.h"
#include "handlebars_map.h"
#include "handlebars_opcodes.h"
#include "handlebars_opcode_serializer.h"
#include "handlebars_parser.h"
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

extern zend_string *HANDLEBARS_INTERNED_STR_LOGGER;
extern zend_string *HANDLEBARS_INTERNED_STR_HELPERS;
extern zend_string *HANDLEBARS_INTERNED_STR_PARTIALS;
extern zend_string *HANDLEBARS_INTERNED_STR_DECORATORS;

static zend_string *INTERNED_CACHE_ID;
static zend_string *INTERNED_ALTERNATE_DECORATORS;
static zend_string *INTERNED_COMPAT;
static zend_string *INTERNED_DATA;
static zend_string *INTERNED_EXPLICIT_PARTIAL_CONTEXT;
static zend_string *INTERNED_IGNORE_STANDALONE;
static zend_string *INTERNED_KNOWN_HELPERS;
static zend_string *INTERNED_KNOWN_HELPERS_ONLY;
static zend_string *INTERNED_PREVENT_INDENT;
static zend_string *INTERNED_STRING_PARAMS;
static zend_string *INTERNED_TRACK_IDS;
static zend_string *INTERNED_STRICT;
static zend_string *INTERNED_ASSUME_OBJECTS;
static zend_string *INTERNED_MUSTACHE_STYLE_LAMBDAS;
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
    zval * z_vm = (zval *) handlebars_vm_get_log_ctx(options->vm);
    zval * logger = zend_read_property_ex(HandlebarsBaseImpl_ce_ptr, z_vm, HANDLEBARS_INTERNED_STR_LOGGER, 1, NULL);
    char * message;
    size_t message_len;
    int i;

    // Generate message
    message = handlebars_talloc_strdup(HANDLEBARS_G(root), "");
    for (i = 0; i < argc; i++) {
        char *tmp = handlebars_value_dump(argv[i], HANDLEBARS_G(root), 0);
        message = handlebars_talloc_asprintf_append_buffer(message, "%s ", tmp);
        handlebars_talloc_free(tmp);
    }
    message_len = talloc_array_length(message) - 1;

    if( logger && Z_TYPE_P(logger) == IS_OBJECT ) {
        HANDLEBARS_VALUE_DECL(level);
        zval z_fn = {0};
        zval z_ret = {0};
        zval z_args[2] = {0};

        if (options->hash) {
            (void) handlebars_value_map_str_find(options->hash, HBS_STRL("level"), level);
        }
        if (level && handlebars_value_get_type(level) == HANDLEBARS_VALUE_TYPE_STRING) {
            ZVAL_STRINGL(&z_fn, handlebars_value_get_strval(level), handlebars_value_get_strlen(level));
        } else {
            ZVAL_STRINGL(&z_fn, "info", sizeof("info") - 1);
        }

        ZVAL_STRINGL(&z_args[0], message, message_len);
        array_init(&z_args[1]);
        call_user_function(&Z_OBJCE_P(logger)->function_table, logger, &z_fn, &z_ret, 2, z_args);
        zval_ptr_dtor(&z_args[1]);
        zval_ptr_dtor(&z_args[0]);
        zval_ptr_dtor(&z_fn);
        zval_ptr_dtor(&z_ret);

        HANDLEBARS_VALUE_UNDECL(level);
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
    obj->helpers = handlebars_talloc_zero_size(obj->context, HANDLEBARS_VALUE_SIZE);
    handlebars_value_map(obj->helpers, handlebars_map_ctor(obj->context, 0));
    obj->partials = handlebars_talloc_zero_size(obj->context, HANDLEBARS_VALUE_SIZE);
    handlebars_value_map(obj->partials, handlebars_map_ctor(obj->context, 0));
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

/* {{{ php_handlebars_compiler_known_helpers_from_zval */
static char ** php_handlebars_compiler_known_helpers_from_zval(void * ctx, zval * arr)
{
    HashTable * data_hash = NULL;
    long count = 0;
    char ** ptr;
    char ** known_helpers;
    zend_string * key;
    zend_ulong index;
    zval * entry = NULL;

    if( !arr || Z_TYPE_P(arr) != IS_ARRAY ) {
        return NULL;
    }

    data_hash = HASH_OF(arr);
    count = zend_hash_num_elements(data_hash);

    if( !count ) {
        return NULL;
    }

    // Allocate array
    ptr = known_helpers = talloc_array(ctx, char *, count + 1);

    // Copy in known helpers
    ZEND_HASH_FOREACH_KEY_VAL(data_hash, index, key, entry) {
        if( Z_TYPE_P(entry) == IS_STRING ) {
            *ptr++ = (char *) handlebars_talloc_strndup(ctx, Z_STRVAL_P(entry), Z_STRLEN_P(entry));
        } else if( key ) {
            *ptr++ = (char *) handlebars_talloc_strndup(ctx, ZSTR_VAL(key), ZSTR_LEN(key));
        } else {
            (void) index;
        }
    } ZEND_HASH_FOREACH_END();

    // Null terminate
    *ptr++ = NULL;

    return known_helpers;
}
/* }}} php_handlebars_compiler_known_helpers_from_zval */

/* {{{ php_handlebars_process_options_zval */
#ifndef Z_IS_TRUE_P
#define Z_IS_TRUE_P(a) (Z_TYPE_P(a) == IS_TRUE)
#endif

#ifndef Z_IS_BOOL_P
#define Z_IS_BOOL_P(a) (Z_TYPE_P(a) == IS_TRUE || Z_TYPE_P(a) == IS_FALSE)
#endif

PHP_HANDLEBARS_API
unsigned long php_handlebars_process_options_zval(struct handlebars_compiler * compiler, struct handlebars_vm * vm, zval * options)
{
    zval * entry;
    HashTable * ht;
    long flags = 0;

    if( !options || Z_TYPE_P(options) != IS_ARRAY ) {
        handlebars_compiler_set_flags(compiler, flags);
        return flags;
    }

    ht = Z_ARRVAL_P(options);
    if( NULL != (entry = zend_hash_find(ht, INTERNED_ALTERNATE_DECORATORS)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_alternate_decorators;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_COMPAT)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_compat;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_DATA)) ) {
        // @todo refine this
        if( !Z_IS_BOOL_P(entry) && Z_TYPE_P(entry) != IS_NULL ) {
            if( vm ) {
                HANDLEBARS_VALUE_DECL(tmp);
                handlebars_value_from_zval(HBSCTX(vm), entry, tmp);
                handlebars_vm_set_data(vm, tmp);
                HANDLEBARS_VALUE_UNDECL(tmp);
            }
            flags |= handlebars_compiler_flag_use_data;
        } else if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_use_data;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_EXPLICIT_PARTIAL_CONTEXT)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_explicit_partial_context;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_IGNORE_STANDALONE)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_ignore_standalone;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_KNOWN_HELPERS)) ) {
        handlebars_compiler_set_known_helpers(compiler, (const char **) php_handlebars_compiler_known_helpers_from_zval(compiler, entry));
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_KNOWN_HELPERS_ONLY)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_known_helpers_only;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_PREVENT_INDENT)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_prevent_indent;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_STRING_PARAMS)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_string_params;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_TRACK_IDS)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_track_ids;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_STRICT)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_strict;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_ASSUME_OBJECTS)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_assume_objects;
        }
    }
    if( NULL != (entry = zend_hash_find(ht, INTERNED_MUSTACHE_STYLE_LAMBDAS)) ) {
        if( Z_IS_TRUE_P(entry) ) {
            flags |= handlebars_compiler_flag_mustache_style_lambdas;
        }
    }

    handlebars_compiler_set_flags(compiler, flags);
    if (vm) {
        handlebars_vm_set_flags(vm, flags);
    }

    return flags;
}
/* }}} php_handlebars_process_options_zval */

/* {{{ php_handlebars_fetch_known_helpers */
HBS_ATTR_NONNULL_ALL
void php_handlebars_fetch_known_helpers(struct handlebars_compiler * compiler, struct handlebars_value * helpers)
{
    const char ** orig = handlebars_compiler_get_known_helpers(compiler);
    long num = 0;
    long idx = 0;
    char ** known_helpers;
    const char ** ptr2;

    // Count the number of builtins
    for( ptr2 = orig ; *ptr2 ; ++ptr2 ) {
        num++;
    }

    // Count the number of helpers
    // this can be wrong?
    // num += handlebars_value_count(helpers);
    HANDLEBARS_VALUE_FOREACH(helpers, child) {
        num++;
        (void) child;
    } HANDLEBARS_VALUE_FOREACH_END();

    // Alloc the array
    known_helpers = handlebars_talloc_array(compiler, char *, num + 1);

    // Copy in the builtins
    for( ptr2 = orig ; *ptr2 ; ++ptr2 ) {
        known_helpers[idx++] = handlebars_talloc_strdup(known_helpers, *ptr2);
    }

    // Copy in the helpers
    HANDLEBARS_VALUE_FOREACH_KV(helpers, key, child) {
        known_helpers[idx++] = handlebars_talloc_strndup(known_helpers, HBS_STR_STRL(key));
        (void) child;
    } HANDLEBARS_VALUE_FOREACH_END();

    known_helpers[idx++] = 0;
    handlebars_compiler_set_known_helpers(compiler, (const char **) known_helpers);
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
    intern->helpers = handlebars_value_from_zval(HBSCTX(context), helpers, intern->helpers);
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
    intern->partials = handlebars_value_from_zval(HBSCTX(context), partials, intern->partials);
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

HBS_ATTR_NONNULL(1, 3, 5)
static struct handlebars_module * compile(
    struct handlebars_context * ctx,
    struct handlebars_vm * vm,
    struct handlebars_string * tmpl,
    struct handlebars_value * helpers,
    zval *z_options
) {
    jmp_buf buf;
    struct handlebars_parser * parser;
    struct handlebars_compiler * compiler;
    struct handlebars_module * module = NULL;
    struct handlebars_ast_node * ast;
    struct handlebars_program * program;
    unsigned long flags;

    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
    parser = handlebars_parser_ctor(ctx);
    compiler = handlebars_compiler_ctor(ctx);

    // Set compiler options
    flags = php_handlebars_process_options_zval(compiler, vm, z_options);
    if( vm && helpers ) {
       php_handlebars_fetch_known_helpers(compiler, helpers);
    }

    // Preprocess template
    handlebars_string_addref(tmpl);
    if( flags & handlebars_compiler_flag_compat ) {
        tmpl = handlebars_preprocess_delimiters(HBSCTX(ctx), tmpl, NULL, NULL);
    }

    // Parse
    php_handlebars_try(HandlebarsCompileException_ce_ptr, parser, &buf);
    ast = handlebars_parse_ex(parser, tmpl, handlebars_compiler_get_flags(compiler));

    // Compile
    php_handlebars_try(HandlebarsCompileException_ce_ptr, compiler, &buf);
    program = handlebars_compiler_compile_ex(compiler, ast);

    module = handlebars_program_serialize(HBSCTX(ctx), program);
    // module->flags = handlebars_compiler_get_flags(compiler); // @todo is this correct?

    handlebars_string_delref(tmpl);

done:
    return module;
}

#define HASH_LEN 8
typedef uint64_t hash_type;

static void hash_pack(hash_type val, char *out) {
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

    hash_type hash = handlebars_hash_xxh3((const char *) UNSAFE_module, size);
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
    } else if (handlebars_module_get_size(UNSAFE_module) != size) {
        zend_throw_exception_ex(HandlebarsInvalidBinaryStringException_ce_ptr, 0,
            "Failed to validate precompiled template: template data segment was %zu bytes, expected %zu",
            size,
            handlebars_module_get_size(UNSAFE_module));
        return NULL;
    } else if (handlebars_module_get_version(UNSAFE_module) != handlebars_version()) {
        zend_throw_exception_ex(HandlebarsInvalidBinaryStringException_ce_ptr, 0,
            "Failed to validate precompiled template: template was compiled with handlebars %d, current version is %d",
            handlebars_module_get_version(UNSAFE_module),
            handlebars_version());
        return NULL;
    }

    struct handlebars_module * module = handlebars_talloc_zero_size(ctx, size);
    talloc_set_type(module, struct handlebars_module);
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

    struct handlebars_module * module = compile(ctx, NULL, tmpl, NULL, z_options);
    if (!module) {
        goto done;
    }

    handlebars_module_normalize_pointers(module, (void *) 1);

    size_t module_size = handlebars_module_get_size(module);
    hash_type hash = handlebars_hash_xxh3((const char *) module, module_size);

    size_t len = HASH_LEN + module_size;
	zend_string *res = zend_string_alloc(len, 0);
    hash_pack(hash, ZSTR_VAL(res));
	memcpy(ZSTR_VAL(res) + HASH_LEN, module, module_size);
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
    jmp_buf buf;
    bool from_cache = false;
    struct handlebars_string * cache_id = NULL;
    struct handlebars_string * input = NULL;
    zval * tmp = NULL;
    HANDLEBARS_VALUE_DECL(context);

    zval *_this_zval = getThis();
    PHP_HBS_ASSERT(_this_zval);
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

    handlebars_vm_set_cache(vm, cache);
    if( intern->helpers ) {
        handlebars_vm_set_helpers(vm, intern->helpers);
    }
    if( intern->partials ) {
        handlebars_vm_set_partials(vm, intern->partials);
    }
    handlebars_vm_set_logger(vm, &php_handlebars_log, _this_zval);

    input = handlebars_string_ctor(HANDLEBARS_G(context), ZSTR_VAL(zstr_input), ZSTR_LEN(zstr_input));
    handlebars_string_addref(input);

    // Check cache id
    if( NULL != z_options
            && NULL != (tmp = zend_hash_find(Z_ARRVAL_P(z_options), INTERNED_CACHE_ID))
            && Z_TYPE_P(tmp) == IS_STRING ) {
        cache_id = handlebars_string_ctor(HANDLEBARS_G(context), Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
        handlebars_string_addref(cache_id);
    } else if (type == input_type_binary) {
        cache_id = NULL;
    } else {
        cache_id = input;
        handlebars_string_addref(cache_id);
    }

    // Lookup cache entry
    if( cache && cache_id && (module = handlebars_cache_find(cache, cache_id)) ) {
        // If we're a file, and stat is enable, check if it's too old
        if( type == input_type_filename && HANDLEBARS_G(cache_stat) ) {
            zval zstat;
            ZVAL_LONG(&zstat, 0);
            php_stat(ZSTR_VAL(zstr_input), ZSTR_LEN(zstr_input), FS_MTIME, &zstat);
            if( Z_LVAL(zstat) > handlebars_module_get_ts(module) ) { // possibly not portable
                handlebars_cache_release(cache, cache_id, module);
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

                stream = php_stream_open_wrapper_ex(hbs_str_val(input), "rb", REPORT_ERRORS, NULL, NULL);
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

        module = compile(ctx, vm, tmpl, intern->helpers, z_options);
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
        handlebars_value_from_zval(HBSCTX(vm), z_context, context);
    }

    // Execute
    struct handlebars_string * vm_buffer = handlebars_vm_execute(vm, module, context);

    if( vm_buffer && !EG(exception) ) {
        HBS_RETVAL_STR(vm_buffer);
    }

done:
    if( from_cache ) {
        handlebars_cache_release(cache, cache_id, module);
    }
    if (input) {
        handlebars_string_delref(input);
    }
    if (cache_id) {
        handlebars_string_delref(cache_id);
    }
    HANDLEBARS_VALUE_UNDECL(context);
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
    INTERNED_ALTERNATE_DECORATORS = zend_new_interned_string(zend_string_init(ZEND_STRL("alternateDecorators"), 1));
    INTERNED_COMPAT = zend_new_interned_string(zend_string_init(ZEND_STRL("compat"), 1));
    INTERNED_DATA = zend_new_interned_string(zend_string_init(ZEND_STRL("data"), 1));
    INTERNED_EXPLICIT_PARTIAL_CONTEXT = zend_new_interned_string(zend_string_init(ZEND_STRL("explicitPartialContext"), 1));
    INTERNED_IGNORE_STANDALONE = zend_new_interned_string(zend_string_init(ZEND_STRL("ignoreStandalone"), 1));
    INTERNED_KNOWN_HELPERS = zend_new_interned_string(zend_string_init(ZEND_STRL("knownHelpers"), 1));
    INTERNED_KNOWN_HELPERS_ONLY = zend_new_interned_string(zend_string_init(ZEND_STRL("knownHelpersOnly"), 1));
    INTERNED_PREVENT_INDENT = zend_new_interned_string(zend_string_init(ZEND_STRL("preventIndent"), 1));
    INTERNED_STRING_PARAMS = zend_new_interned_string(zend_string_init(ZEND_STRL("stringParams"), 1));
    INTERNED_TRACK_IDS = zend_new_interned_string(zend_string_init(ZEND_STRL("trackIds"), 1));
    INTERNED_STRICT = zend_new_interned_string(zend_string_init(ZEND_STRL("strict"), 1));
    INTERNED_ASSUME_OBJECTS = zend_new_interned_string(zend_string_init(ZEND_STRL("assumeObjects"), 1));
    INTERNED_MUSTACHE_STYLE_LAMBDAS = zend_new_interned_string(zend_string_init(ZEND_STRL("mustacheStyleLambdas"), 1));

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
