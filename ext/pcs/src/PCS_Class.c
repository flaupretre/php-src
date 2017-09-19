/*
  +----------------------------------------------------------------------+
  | PCS extension <http://PCS.tekwire.net>                       |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Francois Laupretre <francois@tekwire.net>                    |
  +----------------------------------------------------------------------+
*/

/*---------------------------------------------------------------*/
/* {{{ proto string \PCS\Mgr::__construct() */
/* This private method is never called. It exists only to prevent instanciation
* from user code using a 'new' directive */

static PHP_METHOD(PCS, __construct)
{}

/*---------------------------------------------------------------*/

static PHP_METHOD(PCS, fileCount)
{
	RETURN_LONG(PCS_fileCount);
}

/*---------------------------------------------------------------*/
/* Return array of file info
   Each file info is an array(flags/mode/size/path) */

static PHP_METHOD(PCS, fileInfos)
{
	PCS_Node *node, **nodep;
	char mode[2];
	int i;
	zval zv, elt;
	HashTable *ht;
	zend_string *zs;

	array_init_size(return_value, PCS_fileCount);

	if (PCS_fileCount) {
		mode[1] = '\0';
		for (i=0, nodep=PCS_fileList; i < PCS_fileCount ; i++, nodep++) {
			node = (*nodep);
			mode[0] = PCS_Tree_LoadModeToDisplay(node);

			array_init_size(&elt, 4);
			ht = Z_ARRVAL(elt);

			ZVAL_LONG(&zv, (long)(node->flags));
			zend_hash_str_update(ht, "flags", 5, &zv);

			ZVAL_STRINGL(&zv, mode, 1);
			zend_hash_str_update(ht, "mode", 4, &zv);

			ZVAL_LONG(&zv, (long)(PCS_FILE_LEN(node)));
			zend_hash_str_update(ht, "size", 4, &zv);

			zs = zend_string_alloc(ZSTR_LEN(node->path), 0);
			memcpy(ZSTR_VAL(zs), ZSTR_VAL(node->path), ZSTR_LEN(node->path)+1);
			ZSTR_H(zs) = ZSTR_H(node->path);
			ZVAL_STR(&zv, zs);
			zend_hash_str_update(ht, "path", 4, &zv);

			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &elt);
		}
	}
}

/*---------------------------------------------------------------*/

static zend_function_entry PCS_methods[] = {
	PHP_ME(PCS, __construct, UT_noarg_arginfo, ZEND_ACC_PRIVATE)
	PHP_ME(PCS, fileCount, UT_noarg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, fileInfos, UT_noarg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL, 0, 0}
};

/*===============================================================*/

/*---------------------------------------------------------------*/
/* Declare PCS\Mgr class */

static zend_always_inline int MINIT_PCS_Class()
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PCS\\Mgr", PCS_methods);
	zend_register_internal_class(&ce);

	return SUCCESS;
}

/*===============================================================*/
