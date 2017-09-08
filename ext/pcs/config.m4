
# PCS cannot be disabled

PHP_PCS=yes

AC_DEFINE(HAVE_PCS, 1, [Whether you have the PHP Code Service])

PHP_NEW_EXTENSION(pcs, php_pcs.c, no,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

# PCS must be loaded after tokenizer, pcre, and SPL

PHP_ADD_EXTENSION_DEP(pcs, tokenizer)
PHP_ADD_EXTENSION_DEP(pcs, pcre)
PHP_ADD_EXTENSION_DEP(pcs, spl)

PHP_INSTALL_HEADERS(ext/pcs, client.h)
