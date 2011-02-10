/*****************************************************************************
 *
 * cmd_deinstall.c: Handles packages deinstallation.
 * ----------------
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
#include	<stdlib.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<sys/types.h>
#ifdef	LINUX
#include	<openssl/sha.h>
#else
#include	<sha.h>
#endif
#include	<sys/mman.h>
#include	"include/pakman.h"
#include	"include/file.h"
#include	"include/db.h"
#include	"include/cmd.h"
#include	"include/pkg.h"
#include	"include/error.h"

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
pakman_error_t
cmd_deinstall(int argc, char *argv[], pakman_db_t db)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	char *arg1 = NULL;
	char *arg2 = NULL;
	int force = 0;
	pakman_header_t hdr2;
	char buffer[1024];
	long i;

	/* Start. */
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);

	/* Check Number of arguments. */
	switch(argc)
	{
		case 1:
			arg1 = argv[0];
			break;
		case 2:
			arg1 = argv[0];
			arg2 = argv[1];
			/* Check Value of arguments. */
			if(strcasecmp(arg2, "force"))
			{
				if(strcasecmp(arg2, "noforce"))
				{
					retcode = PAKMAN_ERROR_INVARGS;
					goto cmd_deinstall_end;
				}
			} else {
				force = 1;
			}
			break;
		default:
			retcode = PAKMAN_ERROR_NARGS;
			goto cmd_deinstall_end;
			break;
	}
	if(!strcasecmp(arg1, "base"))
   {
		retcode = PAKMAN_ERROR_PKGPROTECTED;
		goto cmd_deinstall_end;
	}

	/* Try to open DB. */
	fprintf(
		stdout, "%s: Opening database: %s\n",
		__FUNCTION__, PAKMAN_DB_FILEPATH
	);
	db = pakman_db_open();
   if(!db)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_deinstall_end;
   }

   /* See if installed. */
   if(pakman_pkg_dump(db, arg1, &hdr2) == PAKMAN_ERROR_NOERROR)
	{
  		if(pakman_pkg_getstatus(&hdr2) != PAKMAN_PKG_INSTALLED)
  		{
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto cmd_deinstall_end;
		}
  	} else {
		retcode = PAKMAN_ERROR_PKGNINSTALLED;
		goto cmd_deinstall_end;
	}

   /* Unregister it! */
   /* Files. */
	for(i = 0; i < hdr2.files; i++)
	{
	   retcode = pakman_db_get(db, buffer, "PKG_%s_FILE_%ld", arg1, i);
	   if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s_file: Could not read key %s\n",
				__FUNCTION__, buffer
			);
			continue;
	  	}
		fprintf(stdout, "%s: Removing %s\n", __FUNCTION__, buffer);
		pakman_db_del(db, "PKG_%s_FILE_%ld", arg1, i);
		pakman_db_del(db, "PKG_%s_FILE_%ld_FINFO", arg1, i);
		pakman_db_del(db, "PKG_%s_FILE_%ld_SHA", arg1, i);
		pakman_rm(buffer);
	}
   /* Dirs. */
	for(i = 0; i < hdr2.directories; i++)
	{
	   retcode = pakman_db_get(db, buffer, "PKG_%s_DIR_%ld", arg1, i);
	   if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s_file: Could not read key %s\n",
				__FUNCTION__, buffer
			);
			continue;
	  	}
		fprintf(stdout, "%s: Removing %s\n", __FUNCTION__, buffer);
		pakman_db_del(db, "PKG_%s_DIR_%ld", arg1, i);
		pakman_db_del(db, "PKG_%s_DIR_%ld_FINFO", arg1, i);
		pakman_rmdir(buffer);
	}
	/* Done. */
	pakman_db_del(db, "_PKG_%s", arg1);
	pakman_db_del(db, "PKG_%s_STATUS1", arg1);
	pakman_db_del(db, "PKG_%s_STATUS2", arg1);
	pakman_db_del(db, "PKG_%s_VERSION", arg1);
	pakman_db_del(db, "PKG_%s_DIRS", arg1);
	pakman_db_del(db, "PKG_%s_RDATE", arg1);
	pakman_db_del(db, "PKG_%s_FILES", arg1);
	pakman_db_del(db, "PKG_%s_DESCRIPTION", arg1);
	pakman_db_del(db, "PKG_%s_DEPENDS", arg1);
	pakman_db_del(db, "PKG_%s_ARCH", arg1);

	/* Reorganize DB. */
	gdbm_reorganize(db);
cmd_deinstall_end:
	if(db)
		pakman_db_close(db);
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	return retcode;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
