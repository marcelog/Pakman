/*****************************************************************************
 *
 * cmd_query.c: Queries information for a package.
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
cmd_query(int argc, char *argv[], pakman_db_t db)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	char buffer[FILENAME_MAX * 2];
	char *arg1 = NULL;
	char *arg2 = NULL;
	char *arg3 = NULL;
	int chkpkg = 0;
	int chkdeps = 0;
	int chkfiles = 0;
	int chkdirs = 0;
	int chkversion = 0;
	int chkarch = 0;
	int chkinfo = 0;
	int chkrdate = 0;
	int chkidate = 0;
	int chkhome = 0;
	int chkstatus = 0;
	int chkdesc = 0;
	int fd_in = -1;
	struct stat finfo;
	void *filebuf = NULL;
	void *filebuf_end = NULL;
	int i = 0;
	long instdirs = 0;
	long instfiles = 0;
	char *tmpp;
	pakman_fileinfo_t *fhdr;
	pakman_header_t hdr;
	pakman_header_t *hdr2;

	/* Start. */
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);

	/* Check Number of arguments. */
	switch(argc)
	{
		case 3:
			arg1 = argv[0];
			if(!strcasecmp(arg1, "file")) {
				chkpkg = 0;
			} else if(!strcasecmp(arg1, "pkg")) {
				chkpkg = 1;
			} else {
				retcode = PAKMAN_ERROR_INVARGS;
				goto cmd_query_end;
			}
			arg2 = argv[1];
			if(!strcasecmp(arg2, "deps")) {
				chkdeps = 1;
			} else if(!strcasecmp(arg2, "files")) {
				chkfiles = 1;
			} else if(!strcasecmp(arg2, "dirs")) {
				chkdirs = 1;
			} else if(!strcasecmp(arg2, "filesdirs")) {
				chkdirs = 1;
				chkfiles = 1;
			} else if(!strcasecmp(arg2, "version")) {
				chkversion = 1;
			} else if(!strcasecmp(arg2, "arch")) {
				chkarch = 1;
			} else if(!strcasecmp(arg2, "description")) {
				chkdesc = 1;
			} else if(!strcasecmp(arg2, "info")) {
				chkinfo = 1;
			} else if(!strcasecmp(arg2, "rdate")) {
				chkrdate = 1;
			} else if(!strcasecmp(arg2, "idate")) {
				chkidate = 1;
			} else if(!strcasecmp(arg2, "home")) {
				chkhome = 1;
			} else if(!strcasecmp(arg2, "status")) {
				chkstatus = 1;
			} else {
				retcode = PAKMAN_ERROR_INVARGS;
				goto cmd_query_end;
			}
			arg3 = argv[2];
			break;
		default:
			retcode = PAKMAN_ERROR_NARGS;
			goto cmd_query_end;
			break;
	}
	/* See what to do. */
	if(chkpkg > 0)
	{
		/* Try to open DB. */
		db = pakman_db_open();
	   if(!db)
	   {
			retcode = PAKMAN_ERROR_DBERROR;
			goto cmd_query_end;
	   }
	   retcode = pakman_pkg_load(db, arg3, &hdr);
   	if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s: pkg not found: %s\n",
				__FUNCTION__, arg3
			);
			retcode = PAKMAN_ERROR_PKGNINSTALLED;
			goto cmd_query_end;
  		}

  		if(chkdeps)
  		{
  			fprintf(stdout, "%s: DEPS: %s\n", __FUNCTION__, hdr.depends);
		}
		
  		if(chkstatus)
  		{
  			fprintf(stdout, "%s: STATUS1: %s\n", __FUNCTION__, hdr.status1);
  			fprintf(stdout, "%s: STATUS2: %s\n", __FUNCTION__, hdr.status2);
  		}

  		if(chkhome)
  		{
  			fprintf(stdout, "%s: HOME: %s\n", __FUNCTION__, hdr.prefix);
  		}

  		if(chkidate)
  		{
  			fprintf(stdout, "%s: IDATE: %s%c\n", __FUNCTION__, ctime(&hdr.time), 0x8);
  		}

  		if(chkdesc)
  		{
  			fprintf(stdout, "%s: DESCRIPTION: %s\n", __FUNCTION__, hdr.description);
  		}

  		if(chkinfo)
  		{
			pakman_pkg_dump(db, arg3, &hdr);
  		}

  		if(chkrdate)
  		{
  			fprintf(stdout, "%s: RDATE: %s%c\n", __FUNCTION__, ctime(&hdr.time2), 0x8);
  		}

  		if(chkversion)
  		{
  			fprintf(stdout, "%s: VERSION: %s\n", __FUNCTION__, hdr.version);
  		}

  		if(chkarch)
  		{
  			fprintf(stdout, "%s: ARCH: %s\n", __FUNCTION__, hdr.arch);
  		}

 		if(chkfiles)
  		{
			instfiles = hdr.files;
			/* Verify files. */
			for(i = 0; i < instfiles; i++)
			{
			   retcode = pakman_db_get(db, buffer, "PKG_%s_FILE_%ld", arg3, i);
			   if(retcode != PAKMAN_ERROR_NOERROR)
				{
					fprintf(
						stdout, "%s_file: Could not read key %s\n",
						__FUNCTION__, buffer
					);
					retcode = PAKMAN_ERROR_PKGNINSTALLED;
					goto cmd_query_end;
			  	}
				fprintf(stdout, "%s: FILE: %s\n", __FUNCTION__, buffer);
			}
  		}

  		if(chkdirs)
  		{
			instdirs = hdr.directories;
			/* Verify files. */
			for(i = 0; i < instdirs; i++)
			{
			   retcode = pakman_db_get(db, buffer, "PKG_%s_DIR_%ld", arg3, i);
			   if(retcode != PAKMAN_ERROR_NOERROR)
				{
					fprintf(
						stdout, "%s_dir: Could not read key %s\n",
						__FUNCTION__, buffer
					);
					retcode = PAKMAN_ERROR_PKGNINSTALLED;
					goto cmd_query_end;
			  	}
				fprintf(stdout, "%s: DIR: %s\n", __FUNCTION__, buffer);
			}
  		}

	} else {
		sprintf(buffer, "%s", arg3);
		fprintf(stdout, "%s: OK, Proceeding with %s.pak\n", __FUNCTION__, arg3);
		/* Stat package file. */
		if(stat(buffer, &finfo))
	   {
			retcode = PAKMAN_ERROR_OSERROR;
			goto cmd_query_end;
	   }
		
		/* Open package file. */
		fd_in = open(buffer, O_RDONLY);
		if(fd_in < 0)
	   {
			retcode = PAKMAN_ERROR_OSERROR;
			goto cmd_query_end;
	   }
	   /* Map File. */
		filebuf = mmap(
			NULL, finfo.st_size, PROT_READ,
			MAP_SHARED, fd_in, 0
		);
		if(!filebuf)
		{
			close(fd_in);
			retcode = PAKMAN_ERROR_OSERROR;
			goto cmd_query_end;
		}
	   filebuf_end = (char *)filebuf + finfo.st_size;

	   /* Dump header info. */
	   hdr2 = (pakman_header_t *)filebuf;

  		if(chkdeps)
  		{
  			fprintf(stdout, "%s: DEPS: %s\n", __FUNCTION__, hdr2->depends);
		}
  		if(chkhome)
  		{
  			fprintf(stdout, "%s: HOME: %s\n", __FUNCTION__, hdr2->prefix);
  		}

  		if(chkrdate)
  		{
  			fprintf(stdout, "%s: RDATE: %s%c\n", __FUNCTION__, ctime(&hdr2->time), 0x8);
  		}

  		if(chkinfo)
  		{
		   fprintf(stdout, "%s: Name: %s\n", __FUNCTION__, hdr2->name);
		   fprintf(stdout, "%s: Version: %s\n", __FUNCTION__, hdr2->version);
		   fprintf(stdout, "%s: Arch: %s\n", __FUNCTION__, hdr2->arch);
		   fprintf(stdout, "%s: Description: %s\n", __FUNCTION__, hdr2->description);
		   fprintf(stdout, "%s: Created on: %s", __FUNCTION__, ctime(&hdr2->time));
		   fprintf(stdout, "%s: Files: %ld\n", __FUNCTION__, hdr2->files);
		   fprintf(stdout, "%s: Directories: %ld\n", __FUNCTION__, hdr2->directories);
		   fprintf(stdout, "%s: Target: %s\n", __FUNCTION__, hdr2->prefix);
		   fprintf(stdout, "%s: Depends: %s\n", __FUNCTION__, hdr2->depends);
  		}
  		if(chkdesc)
  		{
  			fprintf(stdout, "%s: DESCRIPTION: %s\n", __FUNCTION__, hdr2->description);
  		}

  		if(chkversion)
  		{
  			fprintf(stdout, "%s: VERSION: %s\n", __FUNCTION__, hdr2->version);
  		}

  		if(chkversion)
  		{
  			fprintf(stdout, "%s: ARCH: %s\n", __FUNCTION__, hdr2->arch);
  		}


 		if(chkdirs)
  		{
		   tmpp = (char *)hdr2;
		   tmpp += sizeof(pakman_header_t);
		   fhdr = (pakman_fileinfo_t *)tmpp;
			while((char *)fhdr < (char *)filebuf_end)
			{
				/* Neeeeeeeeeeeext!! */
			   tmpp += sizeof(pakman_fileinfo_t);
				if(S_ISREG(fhdr->finfo.st_mode))
				{
					tmpp += fhdr->finfo.st_size;
				} else if(S_ISLNK(fhdr->finfo.st_mode)) {
					tmpp += strlen(tmpp) + 1;
				} else if(S_ISDIR(fhdr->finfo.st_mode)) {
					/* Print file info. */
					fprintf(
						stdout, "%s: FILE %s\n",
						__FUNCTION__, fhdr->path
					);
				}
				fhdr = (pakman_fileinfo_t *)tmpp;
  			}
  		}

 		if(chkfiles)
  		{
		   tmpp = (char *)hdr2;
		   tmpp += sizeof(pakman_header_t);
		   fhdr = (pakman_fileinfo_t *)tmpp;
			while((char *)fhdr < (char *)filebuf_end)
			{
				/* Neeeeeeeeeeeext!! */
			   tmpp += sizeof(pakman_fileinfo_t);
				if(S_ISREG(fhdr->finfo.st_mode))
				{
					/* Print file info. */
					fprintf(
						stdout, "%s: FILE %s\n",
						__FUNCTION__, fhdr->path
					);
					tmpp += fhdr->finfo.st_size;
				} else if(S_ISLNK(fhdr->finfo.st_mode)) {
					tmpp += strlen(tmpp) + 1;
				}
			   fhdr = (pakman_fileinfo_t *)tmpp;
  			}
  		}


	   /* Unmap File. */
	   munmap(filebuf, finfo.st_size);
	}
	/* Done. */
cmd_query_end:
	if((retcode == PAKMAN_ERROR_NARGS) || (retcode == PAKMAN_ERROR_INVARGS))
		fprintf(
			stdout, "%s: use: <file|pkg> <deps|files|dirs|description|filesdirs|version|arch|rdate|idate|info|home|status> <name>\n",
			__FUNCTION__
		);
	if(fd_in > 0)
		close(fd_in);
	if(db)
		pakman_db_close(db);
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	return retcode;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
