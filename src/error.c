/*****************************************************************************
 *
 * error.c: Error description and handling.
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

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	"include/error.h" 
 
/*****************************************************************************
 * Constants.
 *****************************************************************************/
const char *PAKMAN_ERROR_STR[PAKMAN_ERROR_DUMMY2] = {
	"NULL",
	"No error",
	"Undefined Error",
	"Command not implemented yet",
	"Invalid number of arguments",
	"Invalid arguments",
	"DB Error",
	"DB Already exists, use \"force\" argument",
	"OS Error",
	"SHA MISMATCH!!",
	"File does not exist",
	"Not a directory",
	"Not a regular file",
	"Package installed",
	"Package not installed",
	"Package already installed, use \"force\" flag",
	"File already exists",
	"KEY not found in DB",
	"Dependancy not found",
	"Package is protected"
};

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
const char *
pakman_strerror(pakman_error_t e)
{
	if((e > PAKMAN_ERROR_DUMMY1) && (e < PAKMAN_ERROR_DUMMY2))
		return PAKMAN_ERROR_STR[e];
	else
		return "NULL. Not an errorcode?";
}

/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
