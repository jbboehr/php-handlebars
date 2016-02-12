
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <setjmp.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_types.h"
#include "main/php.h"

#include "handlebars.h"
#include "handlebars_compiler.h"
#include "handlebars_context.h"
#include "handlebars_helpers.h"
#include "handlebars_memory.h"
#include "handlebars_opcode_printer.h"
#include "handlebars_opcodes.h"
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php5to7.h"
#include "php_handlebars.h"

/* {{{ Variables & Prototypes */
zend_class_entry * HandlebarsCompiler_ce_ptr;

static void php_handlebars_compiler_to_zval(struct handlebars_compiler * compiler, zval * current TSRMLS_DC);
/* }}} Variables & Prototypes */

/* {{{ Utils */
static void php_handlebars_operand_append_zval(struct handlebars_operand * operand, zval * arr TSRMLS_DC)
{
    switch( operand->type ) {
        case handlebars_operand_type_null:
            add_next_index_null(arr);
            break;
        case handlebars_operand_type_boolean:
            add_next_index_bool(arr, (int) operand->data.boolval);
            break;
        case handlebars_operand_type_long:
            add_next_index_long(arr, operand->data.longval);
            break;
        case handlebars_operand_type_string:
        	php5to7_add_next_index_string(arr, operand->data.stringval);
            break;
        case handlebars_operand_type_array: {
            _DECLARE_ZVAL(current);
            char ** tmp = operand->data.arrayval;

            _ALLOC_INIT_ZVAL(current);
            array_init(current);

            for( ; *tmp; ++tmp ) {
            	php5to7_add_next_index_string(current, *tmp);
            }

            add_next_index_zval(arr, current);
            break;
        }
    }
}

static void php_handlebars_opcode_to_zval(struct handlebars_opcode * opcode, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(type);
    _DECLARE_ZVAL(args);
    short num;

    _ALLOC_INIT_ZVAL(type);
    PHP5TO7_ZVAL_STRING(type, (char *) handlebars_opcode_readable_type(opcode->type));

    _ALLOC_INIT_ZVAL(args);
    array_init(args);
    num = handlebars_opcode_num_operands(opcode->type);
    if( num >= 1 ) {
        php_handlebars_operand_append_zval(&opcode->op1, args TSRMLS_CC);
    }
    if( num >= 2 ) {
        php_handlebars_operand_append_zval(&opcode->op2, args TSRMLS_CC);
    }
    if( num >= 3 ) {
        php_handlebars_operand_append_zval(&opcode->op3, args TSRMLS_CC);
    }
    if( num >= 4 ) {
        php_handlebars_operand_append_zval(&opcode->op4, args TSRMLS_CC);
    }

	object_init_ex(current, HandlebarsOpcode_ce_ptr);

	do {
		zval z_const, z_ret;
#if PHP_MAJOR_VERSION < 7
		zval **z_const_args = emalloc(2 * sizeof(zval *));

		ZVAL_STRING(&z_const, "__construct", 0);
		z_const_args[0] = type;
		z_const_args[1] = args;

	    call_user_function(&HandlebarsOpcode_ce_ptr->function_table, &current, &z_const, &z_ret, 2, z_const_args TSRMLS_CC);

		efree(z_const_args);
#else
		zval z_const_args[2];

		ZVAL_STRING(&z_const, "__construct");
		z_const_args[0] = *type;
		z_const_args[1] = *args;

		call_user_function(&HandlebarsOpcode_ce_ptr->function_table, current, &z_const, &z_ret, 2, z_const_args TSRMLS_CC);

		zval_dtor(&z_const);
#endif
	} while(0);

	php5to7_zval_ptr_dtor(type);
	php5to7_zval_ptr_dtor(args);
}

static zend_always_inline void php_handlebars_opcodes_to_zval(
    struct handlebars_opcode ** opcodes, size_t count, zval * current TSRMLS_DC)
{
    size_t i;
    struct handlebars_opcode ** pos = opcodes;
    short num;
    _DECLARE_ZVAL(tmp);

    array_init(current);

    for( i = 0; i < count; i++, pos++ ) {
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_opcode_to_zval(*pos, tmp TSRMLS_CC);
        add_next_index_zval(current, tmp);
    }
}

static zend_always_inline void php_handlebars_compilers_to_zval(
    struct handlebars_compiler ** compilers, size_t count, zval * current TSRMLS_DC)
{
    size_t i;
    struct handlebars_compiler * child;
    _DECLARE_ZVAL(tmp);

    array_init(current);

    for( i = 0; i < count; i++ ) {
        child = *(compilers + i);
        _ALLOC_INIT_ZVAL(tmp);
        php_handlebars_compiler_to_zval(child, tmp TSRMLS_CC);
        add_next_index_zval(current, tmp);
    }
}

static void php_handlebars_compiler_to_zval(struct handlebars_compiler * compiler, zval * current TSRMLS_DC)
{
    _DECLARE_ZVAL(opcodes);
    _DECLARE_ZVAL(children);
    _DECLARE_ZVAL(blockParams);

    // Opcodes
    _ALLOC_INIT_ZVAL(opcodes);
    php_handlebars_opcodes_to_zval(compiler->opcodes, compiler->opcodes_length, opcodes TSRMLS_CC);

    // Children
    _ALLOC_INIT_ZVAL(children);
    php_handlebars_compilers_to_zval(compiler->children, compiler->children_length, children TSRMLS_CC);

    // Block params
    _ALLOC_INIT_ZVAL(blockParams);
    ZVAL_LONG(blockParams, compiler->block_params);

    // Construct object
	object_init_ex(current, HandlebarsCompileContext_ce_ptr);

	do {
		zval z_const, z_ret;
#if PHP_MAJOR_VERSION < 7
		zval **z_const_args = emalloc(3 * sizeof(zval *));

		ZVAL_STRING(&z_const, "__construct", 0);
		z_const_args[0] = opcodes;
		z_const_args[1] = children;
		z_const_args[2] = blockParams;

	    call_user_function(&HandlebarsCompileContext_ce_ptr->function_table, &current, &z_const, &z_ret, 3, z_const_args TSRMLS_CC);

		efree(z_const_args);
#else
		zval z_const_args[3];

		ZVAL_STRING(&z_const, "__construct");
		z_const_args[0] = *opcodes;
		z_const_args[1] = *children;
		z_const_args[2] = *blockParams;

		call_user_function(&HandlebarsCompileContext_ce_ptr->function_table, current, &z_const, &z_ret, 3, z_const_args TSRMLS_CC);

		zval_dtor(&z_const);
#endif
	} while(0);

    php5to7_zval_ptr_dtor(opcodes);
    php5to7_zval_ptr_dtor(children);
    php5to7_zval_ptr_dtor(blockParams);

	// Set flags
    if( compiler->result_flags & handlebars_compiler_result_flag_use_depths ) {
        zend_update_property_bool(Z_OBJCE_P(current), current, ZEND_STRL("useDepths"), 1 TSRMLS_CC);
    }
    if( compiler->result_flags & handlebars_compiler_result_flag_use_partial ) {
        zend_update_property_bool(Z_OBJCE_P(current), current, ZEND_STRL("usePartial"), 1 TSRMLS_CC);
    }
//    if( compiler->result_flags & handlebars_compiler_result_flag_is_simple ) {
//    	zend_update_property_bool(Z_OBJCE_P(current), current, ZEND_STRL("isSimple"), 1 TSRMLS_CC);
//    }
    if( compiler->result_flags & handlebars_compiler_result_flag_use_decorators ) {
        zend_update_property_bool(Z_OBJCE_P(current), current, ZEND_STRL("useDecorators"), 1 TSRMLS_CC);
    }
    if( compiler->flags & handlebars_compiler_flag_string_params ) {
        zend_update_property_bool(Z_OBJCE_P(current), current, ZEND_STRL("stringParams"), 1 TSRMLS_CC);
    }
    if( compiler->flags & handlebars_compiler_flag_track_ids ) {
        zend_update_property_bool(Z_OBJCE_P(current), current, ZEND_STRL("trackIds"), 1 TSRMLS_CC);
    }

	// Decorators
	if( compiler->flags & handlebars_compiler_flag_alternate_decorators ) {
	    _DECLARE_ZVAL(decorators);
		_ALLOC_INIT_ZVAL(decorators);
		php_handlebars_compilers_to_zval(compiler->decorators, compiler->decorators_length, decorators TSRMLS_CC);
		zend_update_property(Z_OBJCE_P(current), current, ZEND_STRL("decorators"), decorators TSRMLS_CC);
	    php5to7_zval_ptr_dtor(decorators);
	}
}

static char ** php_handlebars_compiler_known_helpers_from_zval(void * ctx, zval * arr TSRMLS_DC)
{
    HashTable * data_hash = NULL;
    long count = 0;
    char ** ptr;
    const char ** ptr2;
    char ** known_helpers;

    if( !arr || Z_TYPE_P(arr) != IS_ARRAY ) {
        return NULL;
    }

    data_hash = HASH_OF(arr);
    count = zend_hash_num_elements(data_hash);

    if( !count ) {
        return NULL;
    }

    // Count builtins >.>
    for( ptr2 = handlebars_builtins_names(); *ptr2; ++ptr2, ++count );

    // Allocate array
    ptr = known_helpers = talloc_array(ctx, char *, count + 1);

    // Copy in known helpers
    do {
#if PHP_MAJOR_VERSION < 7
        HashPosition data_pointer = NULL;
        zval ** data_entry = NULL;
        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
            if( Z_TYPE_PP(data_entry) == IS_STRING ) {
                *ptr++ = (char *) handlebars_talloc_strdup(ctx, Z_STRVAL_PP(data_entry));
            }
            zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
#else
        zval * data_entry = NULL;
        ZEND_HASH_FOREACH_VAL(data_hash, data_entry) {
            if( Z_TYPE_P(data_entry) == IS_STRING ) {
                *ptr++ = (char *) handlebars_talloc_strdup(ctx, Z_STRVAL_P(data_entry));
            }
        } ZEND_HASH_FOREACH_END();
#endif
    } while(0);

    // Copy in builtins
    for( ptr2 = handlebars_builtins_names(); *ptr2; ++ptr2 ) {
        *ptr++ = (char *) handlebars_talloc_strdup(ctx, *ptr2);
    }

    // Null terminate
    *ptr++ = NULL;

    return known_helpers;
}
/* }}} Utils */

/* {{{ proto mixed Handlebars\Compiler::compile(string tmpl[, long flags[, array knownHelpers]]) */
static inline void php_handlebars_compile(INTERNAL_FUNCTION_PARAMETERS, short print)
{
    char * tmpl;
    strsize_t tmpl_len;
    long compile_flags = 0;
    zval * known_helpers = NULL;
    struct handlebars_context * ctx;
    struct handlebars_compiler * compiler;
    struct handlebars_opcode_printer * printer;
    int retval;
    char * errmsg;
    char ** known_helpers_arr;
    struct zend_class_entry * volatile exception_ce = HandlebarsRuntimeException_ce_ptr;

#ifndef FAST_ZPP
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lz", &tmpl, &tmpl_len, &compile_flags, &known_helpers) == FAILURE ) {
        return;
    }
#else
    ZEND_PARSE_PARAMETERS_START(1, 3)
	    Z_PARAM_STRING(tmpl, tmpl_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(compile_flags)
        Z_PARAM_ZVAL(known_helpers)
    ZEND_PARSE_PARAMETERS_END();
#endif

#if PHP_MAJOR_VERSION >= 7
    // Dereference zval
    if (Z_TYPE_P(known_helpers) == IS_REFERENCE) {
        ZVAL_DEREF(known_helpers);
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

    // Intialize compiler and opcode printer
    compiler = handlebars_compiler_ctor(ctx);
    printer = handlebars_opcode_printer_ctor(compiler);
    handlebars_compiler_set_flags(compiler, compile_flags);

    // Get known helpers
    known_helpers_arr = php_handlebars_compiler_known_helpers_from_zval(compiler, known_helpers TSRMLS_CC);
    if( known_helpers_arr ) {
        compiler->known_helpers = (const char **) known_helpers_arr;
    }

    // Parse
    exception_ce = HandlebarsParseException_ce_ptr;
    ctx->tmpl = tmpl;
    handlebars_parse(ctx);

    // Compile
    exception_ce = HandlebarsCompileException_ce_ptr;
    handlebars_compiler_compile(compiler, ctx->program);

    // Print or convert to zval
    exception_ce = HandlebarsRuntimeException_ce_ptr;
    if( print ) {
        handlebars_opcode_printer_print(printer, compiler);
        PHP5TO7_RETVAL_STRING(printer->output);
    } else {
        php_handlebars_compiler_to_zval(compiler, return_value TSRMLS_CC);
    }

done:
    handlebars_context_dtor(ctx);
}

PHP_METHOD(HandlebarsCompiler, compile)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_METHOD(HandlebarsCompiler, compilePrint)
{
    php_handlebars_compile(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} Handlebars\Compiler::compile */

/* {{{ Argument Info */
ZEND_BEGIN_ARG_INFO_EX(HandlebarsCompiler_compile_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
    ZEND_ARG_INFO(0, flags)
    ZEND_ARG_ARRAY_INFO(0, knownHelpers, 1)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsCompiler methods */
static zend_function_entry HandlebarsCompiler_methods[] = {
	PHP_ME(HandlebarsCompiler, compile, HandlebarsCompiler_compile_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(HandlebarsCompiler, compilePrint, HandlebarsCompiler_compile_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	{ NULL, NULL, NULL }
};
/* }}} HandlebarsCompiler methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_compiler)
{
    zend_class_entry ce;
    int flags = CONST_CS | CONST_PERSISTENT;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Compiler", HandlebarsCompiler_methods);
    HandlebarsCompiler_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("NONE"), handlebars_compiler_flag_none TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("USE_DEPTHS"), handlebars_compiler_flag_use_depths TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("STRING_PARAMS"), handlebars_compiler_flag_string_params TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("TRACK_IDS"), handlebars_compiler_flag_track_ids TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("NO_ESCAPE"), handlebars_compiler_flag_no_escape TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("KNOWN_HELPERS_ONLY"), handlebars_compiler_flag_known_helpers_only TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("PREVENT_INDENT"), handlebars_compiler_flag_prevent_indent TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("EXPLICIT_PARTIAL_CONTEXT"), handlebars_compiler_flag_explicit_partial_context TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("IGNORE_STANDALONE"), handlebars_compiler_flag_ignore_standalone TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("ALTERNATE_DECORATORS"), handlebars_compiler_flag_alternate_decorators TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("COMPAT"), handlebars_compiler_flag_compat TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("ALL"), handlebars_compiler_flag_all TSRMLS_CC);

    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("USE_PARTIAL"), handlebars_compiler_result_flag_use_partial TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("IS_SIMPLE"), handlebars_compiler_result_flag_is_simple TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("USE_DECORATORS"), handlebars_compiler_result_flag_use_decorators TSRMLS_CC);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("RESULT_ALL"), handlebars_compiler_result_flag_all TSRMLS_CC);

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
