/*****************************************************************************
 *
 * cmd_show.c: Displays information about PAKMAN.
 * ----------
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
#include	<gdbm.h>
#include	<string.h>
#include	"include/pakman.h"
#include	"include/db.h"
#include	"include/cmd.h"
#include	"include/pkg.h"
#include	"include/error.h"

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
pakman_error_t
cmd_show(int argc, char *argv[], pakman_db_t db)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	datum key;
	char buffer[32];
	pakman_header_t hdr;

	/* Start. */
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);
	fprintf(stdout, "%s: DB: %s\n", __FUNCTION__, pakman_db_version());

	/* Try to open DB. */
	db = pakman_db_open();
   if(!db)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_verify_end;
   }
	retcode = pakman_db_get(db, buffer, "VERSION");
	fprintf(stdout, "%s: DB Schema Version: %s\n", __FUNCTION__, buffer);
	retcode = pakman_db_get(db, buffer, "DATE");
	fprintf(stdout, "%s: DB Schema Date: %s\n", __FUNCTION__, buffer);

	key = gdbm_firstkey(db);
	fprintf(stdout, "%s: ---- PACKAGES LIST START ----\n", __FUNCTION__);
	while(key.dptr)
	{
		if(!strncmp(key.dptr, "_PKG_", 5))
		{
			fprintf(stdout, "%s: ---- %s PACKAGE ----\n", __FUNCTION__, key.dptr + 5);
			pakman_pkg_dump(db, key.dptr + 5, &hdr);
		}
		key = gdbm_nextkey(db, key);
	}
	fprintf(stdout, "%s: ---- PACKAGES LIST END ----\n", __FUNCTION__);
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	/* Done. */
cmd_verify_end:
	if(db)
		pakman_db_close(db);
	return retcode;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
