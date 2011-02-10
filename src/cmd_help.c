/*****************************************************************************
 *
 * cmd_help.c: Displays Help.
 * -----------
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
#include	<stdio.h>
#include	"include/db.h"
#include	"include/cmd.h"
#include	"include/error.h"

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
pakman_error_t
cmd_help(int argc, char *argv[], pakman_db_t db)
{
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	return PAKMAN_ERROR_NOERROR;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
