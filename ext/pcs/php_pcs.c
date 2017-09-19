/*
  +----------------------------------------------------------------------+
  | PCS extension <http://pcs.tekwire.net>                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2015 The PHP Group                                     |
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

/* Uncomment to display debug messages */
/*#define PCS_DEBUG*/
/* Uncomment to prefix debug messages with timestamps */
/*#define UT_DBG_TIMESTAMPS*/

/*============================================================================*/

#ifdef PCS_DEBUG
#define UT_DEBUG
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_extensions.h"
#include "ext/standard/info.h"

#include "pecl-compat/compat.h"
#include "pecl-utils/utils.h"

/*---------------------------------------------------------------*/

#define MODULE_NAME PHP_PCS_EXTNAME
#define MODULE_VERSION PHP_PCS_VERSION

/*---------------------------------------------------------------*/

#include "php_pcs.h"
#include "./pcs_client.h"

#include "src/PCS_Utils.h"
#include "src/PCS_Tree.h"
#include "src/PCS_Class.h"
#include "src/PCS_Stream.h"
#include "src/PCS_Loader.h"
#include "src/PCS_API.h"

/*------------------------*/
/* Include embedded PHP code */

#include "pcs/phpc/pcs.phpc"

/*------------------------*/

#ifdef COMPILE_DL_PCS
	ZEND_TSRMLS_CACHE_DEFINE();
	ZEND_GET_MODULE(pcs)
#endif

/*------------------------*/
/* Including C code allows to export only the symbols we want to make public */

#include "pecl-utils/utils.c"

#include "src/PCS_Utils.c"
#include "src/PCS_Tree.c"
#include "src/PCS_Class.c"
#include "src/PCS_Stream.c"
#include "src/PCS_Loader.c"
#include "src/PCS_API.c"

/*---------------------------------------------------------------*/
/* phpinfo() output                                              */

static PHP_MINFO_FUNCTION(pcs)
{
	char buf[10];
	zend_ulong modes[2];
	int i;

	php_info_print_table_start();

	php_info_print_table_row(2, "PHP Code Service", "enabled");
	php_info_print_table_row(2, "Version", PHP_PCS_VERSION);

	sprintf(buf, "%d", PCS_fileCount);
	php_info_print_table_row(2, "File count",buf);
	php_info_print_table_end();

	modes[0] = modes[1] = 0;
	if (PCS_fileCount) {
		for (i=0; i < PCS_fileCount ; i++) {
			modes[(PCS_fileList[i]->flags & PCS_LOAD_MASK) -1]++;
		}
	}

	php_info_print_table_start();

	php_info_print_table_colspan_header(2, "Mode");

	sprintf(buf, "%lu", modes[PCS_LOAD_ALWAYS -1]);
	php_info_print_table_row(2, "Always",buf);

	sprintf(buf, "%lu", modes[PCS_LOAD_NONE -1]);
	php_info_print_table_row(2, "",buf);

	php_info_print_table_end();
}

/*---------------------------------------------------------------*/

static PHP_RINIT_FUNCTION(pcs)
{
	DBG_MSG("-> PCS RINIT");
	
	in_startup = 0;

	if (RINIT_PCS_Loader() == FAILURE) return FAILURE;

	DBG_MSG("<- PCS RINIT");
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_RSHUTDOWN_FUNCTION(pcs)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MINIT_FUNCTION(pcs)
{
	DBG_INIT();
	DBG_MSG("-> PCS MINIT");

	if (MINIT_PCS_Tree() == FAILURE) return FAILURE;

	if (MINIT_PCS_Class() == FAILURE) return FAILURE;

	if (MINIT_PCS_Stream() == FAILURE) return FAILURE;

	/* Register PHP code */

	if (PCS_registerEmbedded(pcs_pcs_code, IMM_STRL("ext/pcs"), 0) == FAILURE) {
		return FAILURE;
	}

	DBG_MSG("<- PCS MINIT");
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MSHUTDOWN_FUNCTION(pcs)
{
	if (MSHUTDOWN_PCS_Stream() == FAILURE) return FAILURE;

	if (MSHUTDOWN_PCS_Tree() == FAILURE) return FAILURE;

	return SUCCESS;
}

/*---------------------------------------------------------------*/
/*-- Module definition --*/

zend_module_entry pcs_module_entry = {
	STANDARD_MODULE_HEADER,
	MODULE_NAME,
	NULL,
	PHP_MINIT(pcs),
	PHP_MSHUTDOWN(pcs),
	PHP_RINIT(pcs),
	PHP_RSHUTDOWN(pcs),
	PHP_MINFO(pcs),
	MODULE_VERSION,
	STANDARD_MODULE_PROPERTIES
};

/*---------------------------------------------------------------*/
