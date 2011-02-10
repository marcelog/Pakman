/*****************************************************************************
 *
 * pkg.c: PKG handling code.
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

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include "include/db.h"
#include	"include/pkg.h" 
#include	"include/error.h" 
 
/*****************************************************************************
 * Constants.
 *****************************************************************************/
const char *PAKMAN_PKG_STATUS_STR[PAKMAN_PKG_DUMMY2] = {
	"NULL",
	"INSTALLED",
	"INSTALLING",
	"REMOVED"
};

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
pakman_pstatus_t
pakman_pkg_getstatus(pakman_header_t *hdr)
{
	pakman_pstatus_t i;
	if(!hdr)
		return PAKMAN_ERROR_INVARGS;

	for(i = PAKMAN_PKG_DUMMY1 + 1; i < PAKMAN_PKG_DUMMY2; i++)
		if(!strcmp(pakman_pkg_strstatus(i), hdr->status2))
			break;
	return i == PAKMAN_PKG_DUMMY2 ? 0 : i;
}

pakman_error_t
pakman_pkg_load(pakman_db_t db, const char *name, pakman_header_t *hdr)
{
	char buffer[1024];
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	if(!db || !name || !hdr)
		return PAKMAN_ERROR_INVARGS;

	/* Start. */
	memset(buffer, 0, sizeof(buffer));
	memset(hdr, 0, sizeof(pakman_header_t));
	
   /* Get Install date. */
   retcode = pakman_db_get(db, buffer, "_PKG_%s", name);
	if(retcode == PAKMAN_ERROR_NOERROR)
	{
	  	if(!strcmp(buffer, "0"))
  		{
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto pakman_pkg_load_end;
	  	}
	} else {
		retcode = PAKMAN_ERROR_PKGNINSTALLED;
		goto pakman_pkg_load_end;
	}
	strncat(hdr->name, name, sizeof(hdr->name));
	hdr->time = atoi(buffer);

	/* Get rdate. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_RDATE", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	hdr->time2 = atoi(buffer);

	/* Get version. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_VERSION", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	strncat(hdr->version, buffer, sizeof(hdr->version));

	/* Get arch. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_ARCH", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	strncat(hdr->arch, buffer, sizeof(hdr->arch));


	/* Get Files. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_FILES", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	hdr->files = strtol(buffer, NULL, 10);

	/* Get Directories. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_DIRS", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	hdr->directories = strtol(buffer, NULL, 10);

	/* Get Home/Prefix. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_DIR_0", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	strncat(hdr->prefix, buffer, sizeof(hdr->prefix));

	/* Get Description. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_DESCRIPTION", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	strncat(hdr->description, buffer, sizeof(hdr->description));

	/* Get Status1. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_STATUS1", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	strncat(hdr->status1, buffer, sizeof(hdr->status1));

	/* Get Status2. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_STATUS2", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	strncat(hdr->status2, buffer, sizeof(hdr->status2));

	/* Get Depends. */
	memset(buffer, 0, sizeof(buffer));
   retcode = pakman_db_get(db, buffer, "PKG_%s_DEPENDS", name);
	if(retcode != PAKMAN_ERROR_NOERROR)
		goto pakman_pkg_load_end;
	strncat(hdr->depends, buffer, sizeof(hdr->depends));

	/* Done. */
pakman_pkg_load_end:
	return retcode;
}

pakman_error_t
pakman_pkg_dump(pakman_db_t db, const char *name, pakman_header_t *hdr)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;

	if(!db || !name || !hdr)
		return PAKMAN_ERROR_INVARGS;
	/* Start. */
	retcode = pakman_pkg_load(db, name, hdr);
	if(retcode == PAKMAN_ERROR_NOERROR)
	{
		fprintf(stdout, "%s: Name: %s\n", __FUNCTION__, hdr->name);
		fprintf(stdout, "%s: Version: %s\n", __FUNCTION__, hdr->version);
		fprintf(stdout, "%s: Arch: %s\n", __FUNCTION__, hdr->arch);
		fprintf(
			stdout, "%s: Description: %s\n", __FUNCTION__, hdr->description
		);
		fprintf(
			stdout, "%s: Installed on: %s%c\n",
			__FUNCTION__, ctime(&hdr->time), 0x8
		);
		fprintf(stdout, "%s: Status1: %s\n", __FUNCTION__, hdr->status1);
		fprintf(stdout, "%s: Status2: %s\n", __FUNCTION__, hdr->status2);
		fprintf(stdout, "%s: RDate: %s%c\n", __FUNCTION__, ctime(&hdr->time2), 0x8);
		fprintf(stdout, "%s: Home: %s\n", __FUNCTION__, hdr->prefix);
		fprintf(stdout, "%s: Files: %ld\n", __FUNCTION__, hdr->files);
		fprintf(stdout, "%s: Depends: %s\n", __FUNCTION__, hdr->depends);
		fprintf(
			stdout, "%s: Directories: %ld\n", __FUNCTION__, hdr->directories
		);
	}
	/* Done. */
	return retcode;
}

/* Find the name string for the given command. NULL on error. */
const char *
pakman_pkg_strstatus(pakman_pstatus_t c)
{
	if((c > PAKMAN_PKG_DUMMY1) && (c < PAKMAN_PKG_DUMMY2))
		return PAKMAN_PKG_STATUS_STR[c];
	else
		return "NULL. Not a status?";
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
