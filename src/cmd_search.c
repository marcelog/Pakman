/*****************************************************************************
 *
 * cmd_search.c: Searches for a package.
 * ------------
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
cmd_search(int argc, char *argv[], pakman_db_t db)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	datum key;
	char buffer2[1024];
	char buffer3[1024];
	char buffer4[1024];
	char *arg1 = NULL;
	char *arg2 = NULL;
	int file = 0;

	/* Start. */
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);

	/* Check Number of arguments. */
	switch(argc)
	{
		case 2:
			arg1 = argv[0];
			if(!strcmp(arg1, "file"))
				file = 1;
			else if(!strcmp(arg1, "package"))
				file = 0;
			else
			{
				fprintf(stdout, "%s: use: <file | package> <name>\n", __FUNCTION__);
				retcode = PAKMAN_ERROR_INVARGS;
				goto cmd_search_end;
			}
			arg2 = argv[1];
			break;
		default:
			fprintf(stdout, "%s: use: <file | package> <name>\n", __FUNCTION__);
			retcode = PAKMAN_ERROR_NARGS;
			goto cmd_search_end;
			break;
	}
	/* Try to open DB. */
	db = pakman_db_open();
   if(!db)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_search_end;
   }
   if(file)
   {
   	fprintf(
   		stdout,
   		"%s: Searching file or directory pattern \"*%s*\" in all packages.\n",
   		__FUNCTION__, arg2
   	);
		key = gdbm_firstkey(db);
		while(key.dptr)
		{
			if(strstr(key.dptr, "_FILE_") || strstr(key.dptr, "_DIR_"))
				if(!strstr(key.dptr, "_SHA"))
					if(!strstr(key.dptr, "_FINFO"))
					{
						memset(buffer2, 0, sizeof(buffer2));
						memset(buffer3, 0, sizeof(buffer3));
						pakman_db_get(db, buffer2, key.dptr);
						if(strstr(buffer2, arg2))
						{
							memcpy(
								buffer3, key.dptr + strlen("PKG_"),
								strchr(key.dptr + strlen("PKG_"), '_') -
									(key.dptr + strlen("PKG_"))
							);
							memset(buffer4, 0, sizeof(buffer4));
						   retcode = pakman_db_get(db, buffer4, "%s_SHA", key.dptr);
							fprintf(
								stdout, "%s: %s Found in: \"%s\" SHA: %s\n", __FUNCTION__,
								buffer2, buffer3, buffer4
							);
						}
					}
			key = gdbm_nextkey(db, key);
		}
	}
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	/* Done. */
cmd_search_end:
	if(db)
		pakman_db_close(db);
	return retcode;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
