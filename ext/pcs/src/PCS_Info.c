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

static PHP_METHOD(PCS, fileCount)
{
	RETURN_LONG(zend_hash_num_elements(PCS_fileList));
}

/*---------------------------------------------------------------*/
/* Return array of file info
   Each file info is an array(flags/mode/size/path) */

static PHP_METHOD(PCS, fileInfos)
{
	PCS_Node *node;
	char mode[2];

	array_init_size(return_value, zend_hash_num_elements(PCS_fileList));
	
	mode[1] = '\0';
	ZEND_HASH_FOREACH_PTR(PCS_fileList, node) {
		mode[0] = PCS_Tree_LoadModeToDisplay(node);

		{
		zval zv, elt;
		HashTable *ht;

		array_init_size(&elt, 4);
		ht = Z_ARRVAL(elt);

		ZVAL_LONG(&zv, (long)(node->flags));
		zend_hash_str_update(ht, "flags", 5, &zv);

		ZVAL_STRINGL(&zv, mode, 1);
		zend_hash_str_update(ht, "mode", 4, &zv);

		ZVAL_LONG(&zv, (long)(PCS_FILE_LEN(node)));
		zend_hash_str_update(ht, "size", 4, &zv);

		zend_string_addref(node->path);
		ZVAL_STR(&zv, node->path);
		zend_hash_str_update(ht, "path", 4, &zv);

		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &elt);
		}
	} ZEND_HASH_FOREACH_END();
}

/*===============================================================*/
