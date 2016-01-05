
#if PHP_MAJOR_VERSION < 7

typedef int strsize_t;
typedef long zend_long;

#define PHP5TO7_STRL ZEND_STRS
#define PHP5TO7_RETVAL_STRING(a) RETVAL_STRING(a, 1)
#define PHP5TO7_ZVAL_STRING(z, s) ZVAL_STRING(z, s, 1)

#define php5to7_add_next_index_string(...) add_next_index_string(__VA_ARGS__, 1)
#define php5to7_add_assoc_string(...) add_assoc_string(__VA_ARGS__, 1)
#define php5to7_add_assoc_string_ex(...) add_assoc_string_ex(__VA_ARGS__, 1)
#define php5to7_add_assoc_stringl_ex(...) add_assoc_stringl_ex(__VA_ARGS__, 1)

#define php5to7_register_internal_class_ex(class, parent) zend_register_internal_class_ex(class, parent, NULL TSRMLS_CC)

static inline zval * php5to7_zend_hash_find(const HashTable *ht, const char *arKey, uint nKeyLength) {
	zval ** entry = NULL;
	if( zend_hash_find(ht, arKey, nKeyLength + 1, (void **) &entry) == SUCCESS ) {
		return *entry;
	} else {
		return NULL;
	}
}
static inline zval * php5to7_zend_hash_index_find(const HashTable *ht, ulong h) {
	zval ** entry = NULL;
	if( zend_hash_index_find(ht, h, (void **) &entry) == SUCCESS ) {
		return *entry;
	} else {
		return NULL;
	}
}

#define _DECLARE_ZVAL(name) zval * name
#define _INIT_ZVAL INIT_ZVAL
#define _ALLOC_INIT_ZVAL(name) ALLOC_INIT_ZVAL(name)
#define php5to7_zval_ptr_dtor(z) zval_ptr_dtor((&z))

#else /* PHP_MAJOR_VERSION >= 7 */

typedef size_t strsize_t;

#define PHP5TO7_STRL ZEND_STRL
#define PHP5TO7_RETVAL_STRING(a) RETVAL_STRING(a)
#define PHP5TO7_ZVAL_STRING ZVAL_STRING

#define php5to7_add_next_index_string add_next_index_string
#define php5to7_add_assoc_string(z, k, s) add_assoc_string_ex(z, k, strlen(k)+1, s)
#define php5to7_add_assoc_string_ex add_assoc_string_ex
#define php5to7_add_assoc_stringl_ex add_assoc_stringl_ex

#define php5to7_register_internal_class_ex(class, parent) zend_register_internal_class_ex(class, parent)

#define php5to7_zend_hash_find zend_hash_str_find
#define php5to7_zend_hash_index_find zend_hash_index_find

#define _DECLARE_ZVAL(name) zval name ## _v; zval * name = &name ## _v
#define _INIT_ZVAL ZVAL_NULL
#define _ALLOC_INIT_ZVAL(name) ZVAL_NULL(name)
#define php5to7_zval_ptr_dtor(z) zval_ptr_dtor(z)

#endif

#define _DECLARE_ALLOC_INIT_ZVAL(name) _DECLARE_ZVAL(name); _ALLOC_INIT_ZVAL(name)
