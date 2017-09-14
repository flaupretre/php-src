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

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef PHP_WIN32
# include "config.w32.h"
#else
# include <php_config.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_SCANDIR && HAVE_ALPHASORT && HAVE_DIRENT_H
# include <dirent.h>
#endif

#include "ext/standard/file.h"
#include "ext/standard/php_filestat.h"
#include "main/php_scandir.h"
#include "main/spprintf.h"
#include "main/php.h"
#include "zend_list.h"

/*===============================================================*/
/* Public API */

/*--------------------*/

ZEND_DLEXPORT long PCS_registerEmbedded(PCS_DESCRIPTOR *list
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags)
{
	PCS_Node *node;
	long count;
	char *path;
	size_t path_len;

	if (! in_startup) {
		php_error(E_CORE_ERROR, "PCS_registerEmbedded() can be called during MINIT only");
		return FAILURE;
	}

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		return FAILURE;
	}

	count = 0;
	while (list->data) {
		/* Only descriptor version 0 can be handled */
		if (list->version != 0) {
			php_error(E_CORE_ERROR,"Cannot handle descriptor version (%d)", list->version);
			return FAILURE;
		}

		if (virtual_path && virtual_path_len) {
			spprintf(&path, 0, "%s/%s", virtual_path, list->path);
			path_len = virtual_path_len + list->path_len + 1;
		} else {
			spprintf(&path, 0, "%s", list->path);
			path_len = list->path_len;
		}

		node = PCS_Tree_addFile(path, path_len, list->data, list->data_len, 0, flags);
		EFREE(path);
		if (! node) return FAILURE;
		list++;
		count++;
	}

	return count;
}

/*--------------------*/

ZEND_DLEXPORT PCS_ID PCS_registerData(char *data, size_t data_len
	, const char *path, size_t path_len, zend_ulong flags)
{
	PCS_Node *node;

	if (! in_startup) {
		php_error(E_CORE_ERROR, "PCS_registerData() can be called during MINIT only");
		return NULL;
	}

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		return NULL;
	}

	node = PCS_Tree_addFile(path, path_len, data, data_len, 0, flags);
	return (node ? (PCS_ID)node : NULL);
}

/*--------------------*/
/* Note: Stream wrapper open/opendir functions cannot be used during MINIT */
/* So, we need to revert to stdio functions. */

#define CLEANUP_PCS_registerPath() { \
	if (fp) fclose(fp); \
	PFREE(data); \
	}

#define ABORT_PCS_registerPath() { \
	CLEANUP_PCS_registerPath(); \
	return FAILURE; \
	}

ZEND_DLEXPORT long PCS_registerPath(const char *filename, size_t filename_len
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags)
{
	char *data = NULL, *sub_fname, *sub_vpath, *dname;
	size_t datalen, sub_fname_len, sub_vpath_len;
	long status, fcount = 0;
	FILE *fp = NULL;
	struct stat st;
	struct dirent **namelist;
	int nb, i;

	if (! in_startup) {
		php_error(E_CORE_ERROR, "PCS_registerPath() can be called during MINIT only");
		ABORT_PCS_registerPath();
	}

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		ABORT_PCS_registerPath();
	}


	if (stat(filename, &st)) {
		php_error(E_CORE_ERROR, "%s: %s (errno=%d)", filename, strerror(errno), errno);
		ABORT_PCS_registerPath();
	}

	/* If path is a directory */

	if (S_ISDIR(st.st_mode)) {
		if (! PCS_Tree_addDir(virtual_path, virtual_path_len, flags)) {
			ABORT_PCS_registerPath();
		}
		
		/* Recurse on dir entries */

		nb = php_scandir(filename, &namelist, 0, php_alphasort);
		if (nb < 0) {
			php_error(E_CORE_ERROR,"%s: Cannot scan directory", filename);
			return FAILURE;
		}
		if (nb > 0) {
			for (i = 0; i < nb ; i++) {
				dname = namelist[i]->d_name;
				if ((dname[0] == '.') && ((dname[1] == '\0') || ((dname[1] == '.') || (dname[2] == '\0')))) {
					continue;
				}
				spprintf(&sub_fname, 0, "%s%c%s", filename, PHP_DIR_SEPARATOR, dname);
				sub_fname_len = strlen(sub_fname);
				spprintf(&sub_vpath, 0, "%s/%s", virtual_path, dname);
				sub_vpath_len = strlen(sub_vpath);
				status = PCS_registerPath(sub_fname, sub_fname_len, sub_vpath, sub_vpath_len, flags);
				EFREE(sub_fname);
				EFREE(sub_vpath);
				if (status == FAILURE) {
					/* Fatal error: don't care about namelist mem leak */
					ABORT_PCS_registerPath();
				}
				fcount += status;
				free(namelist[i]);
			}
		free(namelist);
		}

	/* If path is a regular file */

	} else if (S_ISREG(st.st_mode)) {
		
		fp=fopen(filename, "rb");
		if (!fp) {
			php_error(E_CORE_ERROR, "%s: %s (errno=%d)", filename, strerror(errno), errno);
			ABORT_PCS_registerPath();
		}
		fstat(fileno(fp),&st);
		datalen = (size_t)(st.st_size);
		PALLOCATE(data, datalen + 1);
		if (datalen) {
			while (!fread(data, datalen, 1, fp)) {}
		}
		data[datalen]='\0';
		if (! PCS_Tree_addFile(virtual_path, virtual_path_len, data, datalen, 1, flags)) {
			ABORT_PCS_registerPath();
		}
		data = NULL;
		fcount = 1;
	
	/* Other file types are silently ignored */
	}

	CLEANUP_PCS_registerPath();
	return fcount;
}

/*--------------------*/

#define CHECK_INPUT_ID(_func, _ret) \
	if (! id) {	\
		if (throw) {	\
			THROW_EXCEPTION(_func ": Received invalid (null) ID");	\
		}	\
		return _ret;	\
	}

#define CHECK_MODULE_IS_STARTED(_func, _ret) \
	if (in_startup) {	\
		if (throw) {	\
			THROW_EXCEPTION(_func "() cannot be called during MINIT");	\
		}	\
		return _ret;	\
	}	\
	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {	\
		if (throw) {	\
			THROW_EXCEPTION("PCS module is not active");	\
		}	\
		return _ret;	\
	}

/*--------------------*/

ZEND_DLEXPORT int PCS_load(PCS_ID id, int throw)
{
	CHECK_MODULE_IS_STARTED("PCS_load", FAILURE)
	CHECK_INPUT_ID("PCS_load", FAILURE);

	return PCS_Loader_loadNode((PCS_Node *)id, throw);
}

/*--------------------*/

ZEND_DLEXPORT char *PCS_getPathFromID(PCS_ID id, int throw )
{
	CHECK_MODULE_IS_STARTED("PCS_getPathFromID", NULL)
	CHECK_INPUT_ID("PCS_getPathFromID", NULL);

	return ZSTR_VAL(((PCS_Node *)id)->path);
}

/*--------------------*/

ZEND_DLEXPORT char *PCS_getURIFromID(PCS_ID id, int throw )
{
	CHECK_MODULE_IS_STARTED("PCS_getURIFromID", NULL)
	CHECK_INPUT_ID("PCS_getURIFromID", NULL);

	return ZSTR_VAL(((PCS_Node *)id)->uri);
}

/*--------------------*/

ZEND_DLEXPORT PCS_ID PCS_getIDFromPath(const char *path, int throw)
{
	PCS_Node *node;

	CHECK_MODULE_IS_STARTED("PCS_getIDFromPath", NULL)

	node = PCS_Tree_getNodeFromPath(path, strlen(path));
	if (! node) {
		if (throw) {
			THROW_EXCEPTION_1("%s: Node not found", path);
		}
		return NULL;
	}

	return (PCS_ID)node;
}

/*--------------------*/

ZEND_DLEXPORT PCS_ID PCS_getIDFromURI(const char *uri, int throw)
{
	CHECK_MODULE_IS_STARTED("PCS_getIDFromURI", NULL)

	if ((uri[0] != 'p')
		|| (uri[1] != 'c')
		|| (uri[2] != 's')
		|| (uri[3] != ':')
		|| (uri[4] != '/')
		|| (uri[5] != '/')) {
		if (throw) {
			THROW_EXCEPTION_1("%S: Invalid PCS URI (should start with 'pcs://'", uri);
		}
		return NULL;
	}

	return PCS_getIDFromPath(uri + 6, throw);
}

/*===============================================================*/
