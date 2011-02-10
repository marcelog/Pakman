/*****************************************************************************
 *
 * cmd_createdb.c: Handles DB creation.
 * --------------
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
#include	<string.h>
#include	<time.h>

#include	"include/pakman.h"
#include	"include/file.h"
#include	"include/db.h"
#include	"include/cmd.h"
#include	"include/error.h"

/*****************************************************************************
 * External definitions.
 *****************************************************************************/

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
pakman_error_t
cmd_createdb(int argc, char *argv[], pakman_db_t db)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	char *arg = NULL;
	int force = 0;
	char buffer[1024];
	time_t mytime;
	char *pfx = NULL;
	char dbfile[1024];
	
	/* Start. */
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);

	/* Check Number of arguments. */
	switch(argc)
	{
		case 0:
			arg = "noforce";
			break;
		case 1:
			arg = argv[0];
			sprintf(dbfile, "%s", PAKMAN_DB_PATH);
			break;
		case 2:
			arg = argv[0];
			pfx = argv[1];
			sprintf(dbfile, "%s/%s", pfx, PAKMAN_DB_PATH);
			break;
		default:
			retcode = PAKMAN_ERROR_NARGS;
			goto cmd_create_db_end;
			break;
	}

	/* Check Value of arguments. */
	if(strcasecmp(arg, "force"))
	{
		if(strcasecmp(arg, "noforce"))
		{
			retcode = PAKMAN_ERROR_INVARGS;
			goto cmd_create_db_end;
		}
	} else {
		force = 1;
	}
	/* Try to open DB. */
	fprintf(stdout, "%s: Checking for existing database....", __FUNCTION__);
	db = pakman_db_open();
   if(db && !force)
   {
		fprintf(stdout, "%s: found! use force flag.\n", __FUNCTION__);
   	pakman_db_close(db);
		retcode = PAKMAN_ERROR_DBEXISTS;
		goto cmd_create_db_end;
   }
	fprintf(stdout, "OK. Proceeding.\n");

   /* Not existant or force! Create directories */
	fprintf(stdout, "%s: Creating directories.\n", __FUNCTION__);
	retcode = pakman_rmkdir(dbfile, PAKMAN_DB_PATH_MODE, PAKMAN_DB_UID, PAKMAN_DB_GID);
   if(retcode != PAKMAN_ERROR_NOERROR)
		goto cmd_create_db_end;

	/* Remove old database. */
	fprintf(stdout, "%s: Removing old database.\n", __FUNCTION__);
	pakman_rm(dbfile);

	/* Create database. */
	db = pakman_db_create(pfx);
   if(!db)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_create_db_end;
   }
   /* Store it! */
   retcode = pakman_db_set(db, "0.0.1", 6, "VERSION");
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_create_db_end;
   }
   time(&mytime);
   sprintf(buffer, "%s%c", ctime(&mytime), 0x8);
   retcode = pakman_db_set(db, buffer, strlen(buffer) + 1, "DATE");
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_create_db_end;
   }
   
   /* Close database. */
	pakman_db_close(db);
   fprintf(stdout, "%s: Database created.\n", __FUNCTION__);
	/* Done. */
cmd_create_db_end:
	if((retcode == PAKMAN_ERROR_NARGS) || (retcode == PAKMAN_ERROR_INVARGS))
		fprintf(
			stdout, "%s: use: createdb [force|noforce] [path] [prefix]\n",
			__FUNCTION__
		);
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	return retcode;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
