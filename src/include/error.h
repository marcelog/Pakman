/*****************************************************************************
 *
 * error.h: Error description and handling.
 * -------
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
#ifndef	_PAKMAN_ERROR_H
#define	_PAKMAN_ERROR_H

/*****************************************************************************
 * Types and definitions.
 *****************************************************************************/
typedef enum
{
	PAKMAN_ERROR_DUMMY1,
	PAKMAN_ERROR_NOERROR,
	PAKMAN_ERROR_UNDEF,
	PAKMAN_ERROR_NOTIMPL,
	PAKMAN_ERROR_NARGS,
	PAKMAN_ERROR_INVARGS,
	PAKMAN_ERROR_DBERROR,
	PAKMAN_ERROR_DBEXISTS,
	PAKMAN_ERROR_OSERROR,
	PAKMAN_ERROR_SHAFAILS,
	PAKMAN_ERROR_FILENEXISTS,
	PAKMAN_ERROR_NOTDIR,
	PAKMAN_ERROR_NOTFILE,
	PAKMAN_ERROR_PKGINSTALLED,
	PAKMAN_ERROR_PKGNINSTALLED,
	PAKMAN_ERROR_PKGAINSTALLED,
	PAKMAN_ERROR_FILEEXISTS,
	PAKMAN_ERROR_KEYNOTFOUND,
	PAKMAN_ERROR_DEPNOTFOUND,
	PAKMAN_ERROR_PKGPROTECTED,
	PAKMAN_ERROR_DUMMY2
} pakman_error_t;

/*****************************************************************************
 * External definitions.
 *****************************************************************************/
extern const char *pakman_strerror(pakman_error_t e);

#endif

