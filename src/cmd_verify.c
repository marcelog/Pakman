/*****************************************************************************
 *
 * cmd_verify.c: Verifies packages installation/status.
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
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#ifdef	LINUX
#include	<openssl/sha.h>
#else
#include	<sha.h>
#endif
#include	"include/file.h"
#include	"include/db.h"
#include	"include/pkg.h"
#include	"include/cmd.h"
#include	"include/pakman.h"
#include	"include/error.h" 

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
pakman_error_t
cmd_verify(int argc, char *argv[], pakman_db_t db)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	char *arg = NULL;
	char buffer[1024];
	char buffer2[1024];
	char buffer3[1024];	
	char buffer4[1024];	
	long instdirs = 0;
	long instfiles = 0;
	long instdirsfound = 0;
	long instfilesfound = 0;
	long i;
	struct stat finfo;
	struct stat *finfo2 = NULL;
#ifndef	LINUX
	char *flagsstr1, *flagsstr2;
#endif
	int ok = 1;
	pakman_header_t hdr;

	/* Start. */
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);

	/* Check Number of arguments. */
	switch(argc)
	{
		case 1:
			arg = argv[0];
			break;
		default:
			retcode = PAKMAN_ERROR_NARGS;
			goto cmd_verify_end;
			break;
	}

	/* Try to open DB. */
	db = pakman_db_open();
   if(!db)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_verify_end;
   }

   /* See if installed. */
   retcode = pakman_pkg_dump(db, arg, &hdr);
   if(retcode != PAKMAN_ERROR_NOERROR)
	{
		ok = 0;
		retcode = PAKMAN_ERROR_PKGNINSTALLED;
		goto cmd_verify_end;
  	}
	if(pakman_pkg_getstatus(&hdr) != PAKMAN_PKG_INSTALLED)
	{
		ok = 0;
		retcode = PAKMAN_ERROR_PKGNINSTALLED;
		goto cmd_verify_end;
  	}
	instfiles = hdr.files;
	instdirs = hdr.directories;
	
	/* Verify files. */
	for(i = 0; i < instfiles; i++)
	{
	   retcode = pakman_db_get(db, buffer, "PKG_%s_FILE_%ld", arg, i);
	   if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s_file: Could not read key %s\n",
				__FUNCTION__, buffer
			);
			ok = 0;
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto cmd_verify_end;
	  	}
	   retcode = pakman_db_get(db, buffer2, "PKG_%s_FILE_%ld_SHA", arg, i);
	   if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s_file: Could not read key %s\n",
				__FUNCTION__, buffer
			);
			ok = 0;
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto cmd_verify_end;
	  	}
	   retcode = pakman_db_get(db, buffer3, "PKG_%s_FILE_%ld_FINFO", arg, i);
	   if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s_file: Could not read key %s\n",
				__FUNCTION__, buffer
			);
			ok = 0;
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto cmd_verify_end;
	  	}
	  	finfo2 = (struct stat *)buffer3;
	  	/* stat file. */
	  	if(lstat(buffer, &finfo))
	  	{
	  		ok = 0;
			fprintf(
				stdout, "%s_file: Could not access file %s\n",
				__FUNCTION__, buffer
			);
		} else {
			instfilesfound++;
#ifndef	LINUX
			if(finfo.st_flags != finfo2->st_flags) {
		  		ok = 0;
		  		flagsstr1 = fflagstostr(finfo.st_flags);
		  		flagsstr2 = fflagstostr(finfo2->st_flags);
				fprintf(
					stdout, "%s_file %s: FLAGS MISMATCH (%s) vs. (%s).\n",
					__FUNCTION__, buffer,
					flagsstr1 == NULL ? "XXXX" : flagsstr1,
					flagsstr2 == NULL ? "XXXX" : flagsstr2
				);
				if(flagsstr1 != NULL)
					free(flagsstr1);
				if(flagsstr2 != NULL)
					free(flagsstr2);
			}
#endif
			if(finfo.st_mode != finfo2->st_mode) {
		  		ok = 0;
				fprintf(
					stdout, "%s_file %s: MODE MISMATCH %o %o.\n",
					__FUNCTION__, buffer, finfo.st_mode, finfo2->st_mode
				);
			}
			if(finfo.st_size != finfo2->st_size) {
		  		ok = 0;
				fprintf(
					stdout, "%s_file %s: SIZE MISMATCH. %ld %ld\n",
					__FUNCTION__, buffer, (long)finfo.st_size, (long)finfo2->st_size
				);
			}
			if(finfo.st_uid != finfo2->st_uid) {
		  		ok = 0;
				fprintf(
					stdout, "%s_file: %s: UID MISMATCH. %d %d\n",
					__FUNCTION__, buffer, finfo.st_uid, finfo2->st_uid
				);
			}
			if(finfo.st_gid != finfo2->st_gid) {
		  		ok = 0;
				fprintf(
					stdout, "%s_file: %s: GID MISMATCH %d %d\n",
					__FUNCTION__, buffer, finfo.st_gid, finfo2->st_gid
				);
			}
			if(
					(S_ISBLK(finfo.st_mode) || S_ISCHR(finfo.st_mode)) &&
					(finfo.st_rdev != finfo2->st_rdev)
				) {
		  		ok = 0;
				fprintf(
					stdout, "%s_file: %s: RDEV MISMATCH\n",
					__FUNCTION__, buffer
				);
			}
			if(S_ISREG(finfo.st_mode)) {
				/* Check SHA. */
				SHA_File(buffer, buffer4);
				if(strcmp(buffer2, buffer4))
				{
			  		ok = 0;
					fprintf(
						stdout, "%s_file: %s: SHA MISMATCH %s - %s.\n",
						__FUNCTION__, buffer, buffer4, buffer2
					);
				}
			}
		}
	}	

	/* Verify directories. */
	for(i = 0; i < instdirs; i++)
	{
	   retcode = pakman_db_get(db, buffer, "PKG_%s_DIR_%ld", arg, i);
	   if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s_file: Could not read key %s\n",
				__FUNCTION__, buffer
			);
			ok = 0;
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto cmd_verify_end;
	  	}
		if(pakman_exists(buffer) != PAKMAN_FILE_DIRECTORY)
		{
	  		ok = 0;
			fprintf(
				stdout, "%s_dir: %s is not a directory or does not exist!\n",
				__FUNCTION__, buffer
			);
		}
	   retcode = pakman_db_get(db, buffer3, "PKG_%s_DIR_%ld_FINFO", arg, i);
	   if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s_dir: Could not read key %s\n",
				__FUNCTION__, buffer
			);
			ok = 0;
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto cmd_verify_end;
	  	}
	  	finfo2 = (struct stat *)buffer3;
	  	/* stat file. */
	  	if(lstat(buffer, &finfo))
	  	{
	  		ok = 0;
			fprintf(
				stdout, "%s_dir: Could not access file %s\n",
				__FUNCTION__, buffer
			);
		} else {
			instdirsfound++;
			if(finfo.st_mode != finfo2->st_mode) {
		  		ok = 0;
				fprintf(
					stdout, "%s_dir %s: MODE MISMATCH %o %o.\n",
					__FUNCTION__, buffer, finfo.st_mode, finfo2->st_mode
				);
			}
			if(finfo.st_uid != finfo2->st_uid) {
		  		ok = 0;
				fprintf(
					stdout, "%s_dir: %s: UID MISMATCH. %d %d\n",
					__FUNCTION__, buffer, finfo.st_uid, finfo2->st_uid
				);
			}
			if(finfo.st_gid != finfo2->st_gid) {
		  		ok = 0;
				fprintf(
					stdout, "%s_dir: %s: GID MISMATCH %d %d\n",
					__FUNCTION__, buffer, finfo.st_gid, finfo2->st_gid
				);
			}
		}
	}
	fprintf(
		stdout, "%s: %ld total files found.\n",
		__FUNCTION__, instfilesfound
	);
	fprintf(
		stdout, "%s: %ld total directories found.\n",
		__FUNCTION__, instdirsfound
	);
cmd_verify_end:
	if(ok && arg)
		fprintf(
			stdout, "%s: %s is correctly installed.\n", __FUNCTION__, arg
		);
	else if((retcode != PAKMAN_ERROR_PKGNINSTALLED) && arg)
		fprintf(
			stdout, "%s: %s is NOT correctly installed.\n",
			__FUNCTION__, arg
		);
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	return retcode;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
