/*
  +----------------------------------------------------------------------+
  | PCS extension <http://PCS.tekwire.net>						 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2015 The PHP Group									 |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,		 |
  | that is bundled with this package in the file LICENSE, and is		 |
  | available through the world-wide-web at the following url:			 |
  | http://www.php.net/license/3_01.txt.								 |
  | If you did not receive a copy of the PHP license and are unable to	 |
  | obtain it through the world-wide-web, please send a note to			 |
  | license@php.net so we can mail you a copy immediately.				 |
  +----------------------------------------------------------------------+
  | Author: Francois Laupretre <francois@tekwire.net>					 |
  +----------------------------------------------------------------------+
*/

#include "Zend/zend_API.h"
#include "Zend/zend_compile.h"
#include "Zend/zend_execute.h"
#include "Zend/zend_hash.h"
#include "Zend/zend_interfaces.h"

/*==========================================================================*/

/*--------------------*/
/* Execute a PHP script
   Adapted from zend_execute_scripts()
   We use zend_compile_file() and not zend_compile_string() because opcache
   caches PCS URIs.
   Returns SUCCESS|FAILURE
   if throw arg is set, throws exception on error
*/

static int PCS_Loader_loadNode(PCS_Node *node, int throw)
{
	zend_file_handle file_handle;
	zend_op_array *op_array;
	zval zret;

	ZEND_ASSERT(node);
	DBG_MSG1("-> PCS_Loader_loadNode(%s)",ZSTR_VAL(node->path));

	if (!PCS_NODE_IS_FILE(node)) {
		DBG_MSG1("%s: node is not a regular file - load aborted", ZSTR_VAL(node->uri));
		if (throw) {
			THROW_EXCEPTION_1("%s: node is not a regular file - load aborted"
				, ZSTR_VAL(node->uri));
		}
		return FAILURE;
	}
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.handle.fd = 0;
	file_handle.handle.fp = NULL;
	file_handle.filename = ZSTR_VAL(node->uri);
	file_handle.opened_path = NULL;
	file_handle.free_filename = 0;

	op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);
	zend_destroy_file_handle(&file_handle);
	if (!op_array) {
		if (throw) {
			THROW_EXCEPTION_1("%s: Error compiling script - load aborted"
				, ZSTR_VAL(node->uri));
		}
		return FAILURE;
	}

	EG(no_extensions)=1;
	zend_try {
		ZVAL_UNDEF(&zret);
		zend_execute(op_array, &zret);
	} zend_catch {
		if (throw) {
			THROW_EXCEPTION_1("%s: Script execution failure", ZSTR_VAL(node->uri));
		}
		destroy_op_array(op_array);
		EFREE(op_array);
		return FAILURE;
	} zend_end_try();
	EG(no_extensions)=0;

	zval_ptr_dtor(&zret);
	destroy_op_array(op_array);
	EFREE(op_array);

	return SUCCESS;
}

/*===============================================================*/

static zend_always_inline int RINIT_PCS_Loader()
{
	PCS_Node *node, **nodep;
	int i;

	DBG_MSG("Loading scripts marked as PCS_LOAD_AUTO");
	if (PCS_fileCount) {
		for (i=0, nodep=PCS_fileList; i < PCS_fileCount ; i++, nodep++) {
			node = (*nodep);
			if (node->mode == PCS_LOAD_ALWAYS) {
				if (PCS_Loader_loadNode(node, 1) == FAILURE) {
					return FAILURE;
				}
			}
		}
	}

	return SUCCESS;
}

/*===============================================================*/
