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

#ifndef __PCS_CLIENT_H
#define __PCS_CLIENT_H

#include <stdlib.h>

#include "zend.h"
#include "zend_types.h"

/*============================================================================*/

/*-- Load mode --*/

#define PCS_LOAD_AUTO	0		/* Automatically detect PHP scripts */

#define PCS_LOAD_ALWAYS	0x01	/* This code is always available */
#define PCS_LOAD_NONE	0X02	/* PCS does not manage this code */

/*-- */

typedef void * PCS_ID;

/*-- The structure contained in embedded '.phpc' files */

typedef struct {
	int version;		/* Descriptor version (current: 0) */
	char *data;			/* File contents */
	size_t data_len;
	char *path;			/* Virtual path (no leading/trailing '/') */
	size_t path_len;
} PCS_DESCRIPTOR;

/*============================================================================*/
/* Registration functions

  These functions can be called during MINIT only.
*/

/*----------------------------------------------------------------------------*/
/* Registers a descriptor list contained in a '.phpc' file.
   Returns the number of registered scripts (FAILURE on error).
   virtual_path is a string to be used as a prefix for embedded paths.
   By convention, use 'ext/<extension-name>' as 'virtual_path'.
*/

ZEND_DLEXPORT long PCS_registerEmbedded(PCS_DESCRIPTOR *list
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags);

/*----------------------------------------------------------------------------*/
/* Registers a file whose content is already present in memory
   Data is not duplicated. So, it is the caller's responsibility to ensure the
   data is persistent, never overwritten, and freed at MSHUTDOWN if needed.
   Returns the ID of the registered file, or NULL on error.
   virtual_path is mandatory here. By convention, it should start with
   'ext/<extension-name>/'.
*/

ZEND_DLEXPORT PCS_ID PCS_registerData(char *data, size_t data_len
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags);

/*----------------------------------------------------------------------------*/
/* Registers an external file/tree. filename is a path to an existing
   file or directory.
   If it is a directory, the subtree under this directory is recursively crawled
   and registered.
   The source path must be absolute (don't assume anything about the
   current working directory when this function is executed).
   Stream-wrapped paths are not accepted.
   Returns the number of registered scripts, or FAILURE on error.
   virtual_path is mandatory here. By convention, it should start with
   'ext/<extension-name>/'.
*/

ZEND_DLEXPORT long PCS_registerPath(const char *filename, size_t filename_len
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags);

/*============================================================================*/

/*----------------------------------------------------------------------------*/
/*	Load and execute a registered PHP script.
	The input arg is a PCS_ID.
	Use only on unmanaged nodes
	Cannot be called during MINIT.
	Returns SUCCESS|FAILURE.
	If throw arg is set, generates exception on failure
*/

ZEND_DLEXPORT int PCS_load(PCS_ID id, int throw);

/*----------------------------------------------------------------------------*/
/*	Returns the path corresponding to a given ID as a null-terminated string.
	The input arg is a PCS_ID.
	Returns NULL on error.
	Returned string is null-terminated and must NOT be modified nor freed.
	Can be called at any time, even during MINIT.
*/

ZEND_DLEXPORT char *PCS_getPathFromID(PCS_ID id, int throw);

/*----------------------------------------------------------------------------*/
/*	Returns the URI corresponding to a given ID as a null-terminated string.
	The input arg is a PCS_ID.
	Returns NULL on error.
	Returned string is null-terminated and must NOT be modified nor freed.
	Can be called at any time, even during MINIT.
*/

ZEND_DLEXPORT char *PCS_getURIFromID(PCS_ID id, int throw);

/*----------------------------------------------------------------------------*/
/*	Resolves a virtual path to a node ID.
	Path resolution understands most 'usual' path elements : '.', '..', '/' and
	'\' as separators, multiple separators.
	'path' input arg must be null-terminated.
	Returns NULL on error.
	Can be called at any time, even during MINIT.
*/

ZEND_DLEXPORT PCS_ID PCS_getIDFromPath(const char *path, int throw);

/*----------------------------------------------------------------------------*/
/*	Resolves a virtual path to a node ID.
	Path resolution understands most 'usual' path elements : '.', '..', '/' and
	'\' as separators, multiple separators.
	'uri' input arg must be null-terminated.
	Returns NULL on error.
	Can be called at any time, even during MINIT.
*/

ZEND_DLEXPORT PCS_ID PCS_getIDFromURI(const char *uri, int throw);

/*============================================================================*/
#endif /* __PCS_CLIENT_H */
