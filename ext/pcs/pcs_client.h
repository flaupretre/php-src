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

typedef zend_ulong PCS_ID;

/* The structure contained in '.phpc' files */

typedef struct {
	int version;		/* Descriptor version (current: 0) */
	char *data;			/* File contents */
	size_t data_len;
	char *path;			/* Virtual path (no leading/trailing '/') */
	size_t path_len;
} PCS_DESCRIPTOR;

/*============================================================================*/

/*-- Flags --*/

#define PCS_LOAD_MASK	0x03

/* The way the code will be loaded */
/* Default (0) = auto :
	- PHP script: Auto
	- Other file: None
A file is recognized as a PHP script if its content starts with '<?php' */

#define PCS_LOAD_AUTO	0x01	/* Make this code automatically available */
#define PCS_LOAD_NONE	0X02	/* Don't manage this code */

/*============================================================================*/
/* Registration functions

  These functions can be called during MINIT only.
  Set mode to 0 for automatic script detection.
*/

/*----------------------------------------------------------------------------*/
/* Registers a descriptor list contained in a '.phpc' file.
   Returns the number of registered scripts, or FAILURE on error.
   virtual_path is a string to be used as a prefix for embedded paths.
   By convention, use 'ext/<extension-name>' as 'virtual_path'.
*/

ZEND_DLEXPORT long PCS_registerEmbedded(PCS_DESCRIPTOR *list
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags
	,zend_ulong mode);

/*----------------------------------------------------------------------------*/
/* Registers a file already present in memory
   Data is not duplicated. So, it is the caller's responsibility to ensure the
   data is persistent, never overwritten, and freed at MSHUTDOWN if needed.
   Returns the ID of the registered file, or FAILURE on error.
   virtual_path is mandatory here. By convention, it should start with 'ext/<extension-name>/'.
*/

ZEND_DLEXPORT PCS_ID PCS_registerData(char *data, size_t data_len
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags
	,zend_ulong mode);

/*----------------------------------------------------------------------------*/
/* Registers an external file/tree. filename is a path to an existing
   file or directory.
   If it is a directory, the subtree is recursively crawled and registered.
   The source path must be absolute (don't assume anything about the
   current working directory when this function is executed).
   Stream-wrapped paths are not accepted.
   Returns the number of registered scripts, or FAILURE on error.
   virtual_path is mandatory here. By convention, it should start with 'ext/<extension-name>/'.
*/

ZEND_DLEXPORT long PCS_registerPath(const char *filename, size_t filename_len
	, const char *virtual_path, size_t virtual_path_len, zend_ulong flags
	,zend_ulong mode);

/*============================================================================*/
/* Misc. functions

These functions are for advanced cases only (module introspection or explicit
management of scripts).
*/

/*----------------------------------------------------------------------------*/
/*	Execute a registered PHP script.
	The input arg is a PCS_ID.
	Use only on PCS_LOAD_NONE nodes
	Cannot be called during MINIT.
	Returns SUCCESS|FAILURE.
	If throw arg is set, generates exception on failure
*/

ZEND_DLEXPORT int PCS_loadScript(PCS_ID id, int throw TSRMLS_DC);

/*----------------------------------------------------------------------------*/
/*	Returns the path corresponding to a given ID as a null-terminated string.
	The input arg is a PCS_ID.
	Returns NULL on error.
	Returned string must NOT be modified nor freed.
	Can be called at any time, even during MINIT.
*/

ZEND_DLEXPORT char *PCS_getPath(PCS_ID id);

/*----------------------------------------------------------------------------*/
/*	Returns the ID of a registered script, knowing its virtual path
	Returns FAILURE on error.
	Can be called at any time, even during MINIT.
*/

ZEND_DLEXPORT PCS_ID PCS_getID(const char *virtual_path, size_t virtual_path_len);

/*============================================================================*/
#endif /* __PCS_CLIENT_H */
