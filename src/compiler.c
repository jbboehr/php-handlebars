
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <setjmp.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_types.h"
#include "main/php.h"

#define HANDLEBARS_OPCODES_PRIVATE
#define HANDLEBARS_COMPILER_PRIVATE

#include "handlebars.h"
#include "handlebars_memory.h"
#include "handlebars_compiler.h"
#include "handlebars_delimiters.h"
#include "handlebars_helpers.h"
#include "handlebars_parser.h"
#include "handlebars_opcode_printer.h"
#include "handlebars_opcodes.h"
#include "handlebars_string.h"
#include "handlebars_value.h"
#include "handlebars_vm.h"

#define BOOLEAN HBS_BOOLEAN
#include "handlebars.tab.h"
#include "handlebars.lex.h"

#include "php_handlebars.h"

#include "php7to8.h"

/* {{{ Variables & Prototypes */
PHP_HANDLEBARS_API zend_class_entry * HandlebarsCompiler_ce_ptr;

static void php_handlebars_program_to_zval(struct handlebars_program * program, zval * current);
/* }}} Variables & Prototypes */

/* {{{ Utils */
static void php_handlebars_operand_append_zval(struct handlebars_operand * operand, zval * arr)
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
        	add_next_index_stringl(arr, HBS_STR_STRL(operand->data.string.string));
            break;
        case handlebars_operand_type_array: {
            zval current = {0};
            int len = operand->data.array.count;
            int i;
            struct handlebars_operand_string * tmp = operand->data.array.array;

            ZVAL_NULL(&current);
            array_init(&current);

            for( i = 0 ; i < len; ++i ) {
                add_next_index_stringl(&current, HBS_STR_STRL(tmp[i].string));
            }

            add_next_index_zval(arr, &current);
            break;
        }
        default: assert(0); break;
    }
}

static void php_handlebars_opcode_to_zval(struct handlebars_opcode * opcode, zval * current)
{
    zval type = {0};
    zval args = {0};
    zval z_const = {0};
    zval z_ret = {0};
    zval z_const_args[2];
    short num;

    ZVAL_NULL(&type);
    ZVAL_STRING(&type, (char *) handlebars_opcode_readable_type(opcode->type));

    ZVAL_NULL(&args);
    array_init(&args);
    num = handlebars_opcode_num_operands(opcode->type);
    if( num >= 1 ) {
        php_handlebars_operand_append_zval(&opcode->op1, &args);
    }
    if( num >= 2 ) {
        php_handlebars_operand_append_zval(&opcode->op2, &args);
    }
    if( num >= 3 ) {
        // hack for invoke_ambiguous
        if (opcode->type == handlebars_opcode_type_invoke_ambiguous && opcode->op3.type == handlebars_operand_type_null) {
            // ignore
        } else {
            php_handlebars_operand_append_zval(&opcode->op3, &args);
        }
    }
    if( num >= 4 ) {
        php_handlebars_operand_append_zval(&opcode->op4, &args);
    }

	object_init_ex(current, HandlebarsOpcode_ce_ptr);

    ZVAL_STRING(&z_const, "__construct");
    z_const_args[0] = type;
    z_const_args[1] = args;

    call_user_function(&HandlebarsOpcode_ce_ptr->function_table, current, &z_const, &z_ret, 2, z_const_args);

    zval_ptr_dtor(&z_const);

	zval_ptr_dtor(&type);
	zval_ptr_dtor(&args);
}

static zend_always_inline void php_handlebars_opcodes_to_zval(
    struct handlebars_opcode ** opcodes, size_t count, zval * current)
{
    size_t i;
    struct handlebars_opcode ** pos = opcodes;
    zval tmp = {0};

    array_init(current);

    for( i = 0; i < count; i++, pos++ ) {
        ZVAL_NULL(&tmp);
        php_handlebars_opcode_to_zval(*pos, &tmp);
        add_next_index_zval(current, &tmp);
    }
}

static zend_always_inline void php_handlebars_programs_to_zval(
    struct handlebars_program ** programs, size_t count, zval * current)
{
    size_t i;
    struct handlebars_program * child;
    zval tmp = {0};

    array_init(current);

    for( i = 0; i < count; i++ ) {
        child = *(programs + i);
        ZVAL_NULL(&tmp);
        php_handlebars_program_to_zval(child, &tmp);
        add_next_index_zval(current, &tmp);
    }
}

static void php_handlebars_program_to_zval(struct handlebars_program * program, zval * current)
{
    zval opcodes = {0};
    zval children = {0};
    zval blockParams = {0};
    zval z_const = {0};
    zval z_ret = {0};
    zval z_const_args[3];

    // Opcodes
    ZVAL_NULL(&opcodes);
    php_handlebars_opcodes_to_zval(program->opcodes, program->opcodes_length, &opcodes);

    // Children
    ZVAL_NULL(&children);
    php_handlebars_programs_to_zval(program->children, program->children_length, &children);

    // Block params
    ZVAL_NULL(&blockParams);
    ZVAL_LONG(&blockParams, program->block_params);

    // Construct object
	object_init_ex(current, HandlebarsProgram_ce_ptr);

    ZVAL_STRING(&z_const, "__construct");
    z_const_args[0] = opcodes;
    z_const_args[1] = children;
    z_const_args[2] = blockParams;

    call_user_function(&HandlebarsProgram_ce_ptr->function_table, current, &z_const, &z_ret, 3, z_const_args);

    zval_ptr_dtor(&z_const);

    zval_ptr_dtor(&opcodes);
    zval_ptr_dtor(&children);
    zval_ptr_dtor(&blockParams);

	// Set flags
    if( program->result_flags & handlebars_compiler_result_flag_use_depths ) {
        zend_update_property_bool(Z_OBJCE_P(current), PHP7TO8_Z_OBJ_P(current), ZEND_STRL("useDepths"), 1);
    }
    if( program->result_flags & handlebars_compiler_result_flag_use_partial ) {
        zend_update_property_bool(Z_OBJCE_P(current), PHP7TO8_Z_OBJ_P(current), ZEND_STRL("usePartial"), 1);
    }
//    if( compiler->result_flags & handlebars_compiler_result_flag_is_simple ) {
//    	zend_update_property_bool(Z_OBJCE_P(current), current, ZEND_STRL("isSimple"), 1);
//    }
    if( program->result_flags & handlebars_compiler_result_flag_use_decorators ) {
        zend_update_property_bool(Z_OBJCE_P(current), PHP7TO8_Z_OBJ_P(current), ZEND_STRL("useDecorators"), 1);
    }
    if( program->flags & handlebars_compiler_flag_string_params ) {
        zend_update_property_bool(Z_OBJCE_P(current), PHP7TO8_Z_OBJ_P(current), ZEND_STRL("stringParams"), 1);
    }
    if( program->flags & handlebars_compiler_flag_track_ids ) {
        zend_update_property_bool(Z_OBJCE_P(current), PHP7TO8_Z_OBJ_P(current), ZEND_STRL("trackIds"), 1);
    }

	// Decorators
	if( program->flags & handlebars_compiler_flag_alternate_decorators ) {
	    zval decorators = {0};
	    ZVAL_NULL(&decorators);
		php_handlebars_programs_to_zval(program->decorators, program->decorators_length, &decorators);
		zend_update_property(Z_OBJCE_P(current), PHP7TO8_Z_OBJ_P(current), ZEND_STRL("decorators"), &decorators);
	    zval_ptr_dtor(&decorators);
	}
}
/* }}} Utils */

/* {{{ proto mixed Handlebars\Compiler::compile(string tmpl[, long flags[, array knownHelpers]]) */
static void php_handlebars_compile(INTERNAL_FUNCTION_PARAMETERS, short print)
{
    zend_string * tmpl = NULL;
    zval * options = NULL;
    TALLOC_CTX * volatile mctx = NULL;
    struct handlebars_context * ctx;
    struct handlebars_parser * parser;
    struct handlebars_compiler * compiler;
    struct handlebars_string * tmpl_str;
    struct handlebars_ast_node * ast;
    struct handlebars_program * program;
    zend_long pool_size = HANDLEBARS_G(pool_size);
    jmp_buf buf;
    unsigned long flags;

    ZEND_PARSE_PARAMETERS_START(1, 2)
	    Z_PARAM_STR(tmpl)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(options)
    ZEND_PARSE_PARAMETERS_END();

    // Dereference zval
    if (options && Z_TYPE_P(options) == IS_REFERENCE) {
        ZVAL_DEREF(options);
    }

    // Initialize context
    if( pool_size <= 0 ) {
        ctx = handlebars_context_ctor_ex(HANDLEBARS_G(root));
    } else {
        mctx = talloc_pool(HANDLEBARS_G(root), pool_size);
        ctx = handlebars_context_ctor_ex(mctx);
    }

    // Initialize parser and compiler
    php_handlebars_try(HandlebarsRuntimeException_ce_ptr, ctx, &buf);
    parser = handlebars_parser_ctor(ctx);
    compiler = handlebars_compiler_ctor(ctx);

    // Set compiler flags
    flags = php_handlebars_process_options_zval(compiler, NULL, options);

    // Preprocess template
    tmpl_str = handlebars_string_ctor(HBSCTX(parser), ZSTR_VAL(tmpl), ZSTR_LEN(tmpl));
    php_handlebars_try(HandlebarsCompileException_ce_ptr, parser, &buf);
    if( flags & handlebars_compiler_flag_compat ) {
        tmpl_str = handlebars_preprocess_delimiters(HBSCTX(ctx), tmpl_str, NULL, NULL);
    }

    // Parse
    php_handlebars_try(HandlebarsCompileException_ce_ptr, parser, &buf);
    ast = handlebars_parse_ex(parser, tmpl_str, flags);

    // Compile
    php_handlebars_try(HandlebarsCompileException_ce_ptr, compiler, &buf);

    program = handlebars_compiler_compile_ex(compiler, ast);

    // Print or convert to zval
    if( print ) {
        struct handlebars_string * tmp = handlebars_program_print(ctx, program, 0);
        HBS_RETVAL_STR(tmp);
        handlebars_talloc_free(tmp);
    } else {
        php_handlebars_program_to_zval(program, return_value);
    }

done:
    handlebars_context_dtor(ctx);
    talloc_free(mctx);
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
PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsCompiler, compile, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

PHP_HANDLEBARS_BEGIN_ARG_WITH_RETURN_TYPE_INFO(HandlebarsCompiler, compilePrint, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()
/* }}} Argument Info */

/* {{{ HandlebarsCompiler methods */
static zend_function_entry HandlebarsCompiler_methods[] = {
	PHP_ME(HandlebarsCompiler, compile, arginfo_HandlebarsCompiler_compile, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(HandlebarsCompiler, compilePrint, arginfo_HandlebarsCompiler_compilePrint, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_FE_END
};
/* }}} HandlebarsCompiler methods */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(handlebars_compiler)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Handlebars\\Compiler", HandlebarsCompiler_methods);
    HandlebarsCompiler_ce_ptr = zend_register_internal_class(&ce);

    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("NONE"), handlebars_compiler_flag_none);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("USE_DEPTHS"), handlebars_compiler_flag_use_depths);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("STRING_PARAMS"), handlebars_compiler_flag_string_params);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("TRACK_IDS"), handlebars_compiler_flag_track_ids);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("NO_ESCAPE"), handlebars_compiler_flag_no_escape);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("KNOWN_HELPERS_ONLY"), handlebars_compiler_flag_known_helpers_only);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("PREVENT_INDENT"), handlebars_compiler_flag_prevent_indent);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("EXPLICIT_PARTIAL_CONTEXT"), handlebars_compiler_flag_explicit_partial_context);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("IGNORE_STANDALONE"), handlebars_compiler_flag_ignore_standalone);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("ALTERNATE_DECORATORS"), handlebars_compiler_flag_alternate_decorators);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("STRICT"), handlebars_compiler_flag_strict);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("ASSUME_OBJECTS"), handlebars_compiler_flag_assume_objects);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("MUSTACHE_STYLE_LAMBDAS"), handlebars_compiler_flag_mustache_style_lambdas);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("COMPAT"), handlebars_compiler_flag_compat);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("ALL"), handlebars_compiler_flag_all);

    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("USE_PARTIAL"), handlebars_compiler_result_flag_use_partial);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("IS_SIMPLE"), handlebars_compiler_result_flag_is_simple);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("USE_DECORATORS"), handlebars_compiler_result_flag_use_decorators);
    zend_declare_class_constant_long(HandlebarsCompiler_ce_ptr, ZEND_STRL("RESULT_ALL"), handlebars_compiler_result_flag_all);

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
