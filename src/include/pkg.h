/*****************************************************************************
 *
 * pkg.h: PKG handling utilities.
 * -----
 *
 * 
 * NOTES:
 * -----
 *		+ Best viewed with tabstop=3.
 *
 *
 * ISSUES:
 * ------
 *
 *
 * Author: Marcelo Gornstein <marcelog@gmail.com>
 * ------
 *
 *****************************************************************************/
#ifndef	_PAKMAN_PKG_H
#define	_PAKMAN_PKG_H

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	"error.h"
#include	"pakman.h"

/*****************************************************************************
 * Types and Definitions.
 *****************************************************************************/
typedef enum
{
	PAKMAN_PKG_DUMMY1,
	PAKMAN_PKG_INSTALLED,
	PAKMAN_PKG_INSTALLING,
	PAKMAN_PKG_REMOVED,
	PAKMAN_PKG_DUMMY2
} pakman_pstatus_t;

/*****************************************************************************
 * External definitions.
 *****************************************************************************/
extern const char *PAKMAN_PKG_STATUS_STR[PAKMAN_PKG_DUMMY2];

extern pakman_pstatus_t pakman_pkg_getstatus(pakman_header_t *hdr);

extern const char *pakman_pkg_strstatus(pakman_pstatus_t c);

extern pakman_error_t
pakman_pkg_load(pakman_db_t db, const char *name, pakman_header_t *hdr);

extern pakman_error_t
pakman_pkg_dump(pakman_db_t db, const char *name, pakman_header_t *hdr);

#endif
