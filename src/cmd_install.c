/*****************************************************************************
 *
 * cmd_install.c: Handles package installation.
 * -------------
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
 * Author: Marcelo Gornstein <marcelog@gmai.com>
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
#include	<libgen.h>
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
cmd_install(int argc, char *argv[], pakman_db_t db)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	char *arg1 = NULL;
	char *arg2 = NULL;
	char *arg3 = NULL;
	int force = 0;
	char buffer[1024];
	char buffer2[1024];
	char buffer3[1024];
	char tmpfilename[1024];
	time_t mytime;
	void *filebuf = NULL;
	pakman_header_t hdr2;
	pakman_header_t *hdr = NULL;
	int fd_in = -1;
	int fd_out;
	struct stat finfo;
	long instfiles = 0;
	long instdirs = 0;
	pakman_fileinfo_t *fhdr;
	void *filebuf_end = NULL;
	char *tmpp;
	char *tmpstr1, *tmpstr2;
	char *flagsstr = NULL;

	/* Start. */
	umask(000000);
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
					goto cmd_install_end;
				}
			} else {
				force = 1;
			}
			break;
		case 3:
			arg1 = argv[0];
			arg2 = argv[1];
			arg3 = argv[2];
			/* Check Value of arguments. */
			if(strcasecmp(arg2, "force"))
			{
				if(strcasecmp(arg2, "noforce"))
				{
					retcode = PAKMAN_ERROR_INVARGS;
					goto cmd_install_end;
				}
			} else {
				force = 1;
			}
			break;
		case 4:
			arg1 = argv[0];
			arg2 = argv[1];
			arg3 = argv[2];
			PAKMAN_DB_FILEPATH = argv[3];
			/* Check Value of arguments. */
			if(strcasecmp(arg2, "force"))
			{
				if(strcasecmp(arg2, "noforce"))
				{
					retcode = PAKMAN_ERROR_INVARGS;
					goto cmd_install_end;
				}
			} else {
				force = 1;
			}
			break;
		default:
			retcode = PAKMAN_ERROR_NARGS;
			goto cmd_install_end;
			break;
	}

	/* Try to open DB. */
	db = pakman_db_open();
   if(!db)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }

	sprintf(buffer, "%s", arg1);
	fprintf(stdout, "%s: OK, Proceeding with %s.pak\n", __FUNCTION__, arg1);
	/* Stat package file. */
	if(stat(buffer, &finfo))
   {
		retcode = PAKMAN_ERROR_OSERROR;
		goto cmd_install_end;
   }
	
	/* Open package file. */
	fd_in = open(buffer, O_RDONLY);
	if(fd_in < 0)
   {
		retcode = PAKMAN_ERROR_OSERROR;
		goto cmd_install_end;
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
		goto cmd_install_end;
	}
   filebuf_end = (char *)filebuf + finfo.st_size;

   /* Dump header info. */
   hdr = (pakman_header_t *)filebuf;
   fprintf(stdout, "%s: Name: %s\n", __FUNCTION__, hdr->name);
   fprintf(stdout, "%s: Version: %s\n", __FUNCTION__, hdr->version);
   fprintf(stdout, "%s: Arch: %s\n", __FUNCTION__, hdr->arch);
   fprintf(stdout, "%s: Description: %s\n", __FUNCTION__, hdr->description);
   fprintf(stdout, "%s: Created on: %s", __FUNCTION__, ctime(&hdr->time));
   fprintf(stdout, "%s: Files: %ld\n", __FUNCTION__, hdr->files);
   fprintf(stdout, "%s: Directories: %ld\n", __FUNCTION__, hdr->directories);
   fprintf(stdout, "%s: Target: %s\n", __FUNCTION__, hdr->prefix);
   fprintf(stdout, "%s: Depends: %s\n", __FUNCTION__, hdr->depends);

   /* See if installed. */
   if(pakman_pkg_dump(db, hdr->name, &hdr2) == PAKMAN_ERROR_NOERROR)
	{
  		if(!force && (pakman_pkg_getstatus(&hdr2) == PAKMAN_PKG_INSTALLED))
  		{
			retcode = PAKMAN_ERROR_PKGAINSTALLED;
			goto cmd_install_end;
		}
  	}
	/* Check dependencies. */
	tmpstr1 = hdr->depends;
	tmpstr2 = strchr(hdr->depends, ' ');
	if(!tmpstr2)
		tmpstr2 = tmpstr1 + strlen(tmpstr1);
	while(tmpstr1 && (tmpstr1 != tmpstr2))
	{
		memset(buffer3, 0, sizeof(buffer3));
		memcpy(buffer3, tmpstr1, tmpstr2 - tmpstr1);
	   retcode = pakman_pkg_load(db, buffer3, &hdr2);
   	if(retcode != PAKMAN_ERROR_NOERROR)
		{
			fprintf(
				stdout, "%s: dependancy not found: %s\n",
				__FUNCTION__, buffer3
			);
			retcode = PAKMAN_ERROR_DEPNOTFOUND;
			goto cmd_install_end;
  		}
		if(pakman_pkg_getstatus(&hdr2) != PAKMAN_PKG_INSTALLED)
		{
			fprintf(
				stdout, "%s: dependancy not found: %s\n",
				__FUNCTION__, pakman_pkg_strstatus(pakman_pkg_getstatus(&hdr2))
			);
			retcode = PAKMAN_ERROR_DEPNOTFOUND;
			goto cmd_install_end;
	  	}
		tmpstr1 = tmpstr2 + 1;
		tmpstr2 = strchr(tmpstr1, ' ');
		if(!tmpstr2)
			tmpstr2 = tmpstr1 + strlen(tmpstr1);
		fprintf(
			stdout, "%s: dependancy found: %s\n", __FUNCTION__, buffer3
		);
	}
  	/* Store package name and install date=0. */
	retcode = pakman_db_set(db, "0", 2, "_PKG_%s", hdr->name);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
  	/* Store package status2. */
	retcode =
		pakman_db_set(
			db, (char *)pakman_pkg_strstatus(PAKMAN_PKG_INSTALLING),
			strlen(pakman_pkg_strstatus(PAKMAN_PKG_INSTALLING)) + 1,
			"PKG_%s_STATUS2", hdr->name
		)
	;
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }

   /* Traverse Directories. */
   tmpp = filebuf;
   tmpp += sizeof(pakman_header_t);
   fhdr = (pakman_fileinfo_t *)tmpp;
	fprintf(
		stdout, "%s: %p %p %p\n",
		__FUNCTION__, filebuf, (void *)fhdr, filebuf_end
	);
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
#ifndef	LINUX
			flagsstr = fflagstostr(fhdr->finfo.st_flags);
#endif
			fprintf(
				stdout, "%s_dir: %s, sha: %s, size: %ld, mode: %o, ownership: %ld:%ld, flags: %s\n",
				__FUNCTION__, fhdr->path, fhdr->sha,
				(long)fhdr->finfo.st_size, fhdr->finfo.st_mode, 
				(long)fhdr->finfo.st_uid, (long)fhdr->finfo.st_gid,
				flagsstr == NULL ? "XXXX" : flagsstr
			);
			if(flagsstr != NULL)
				free(flagsstr);
			if(!arg3)
				sprintf(buffer, "%s", fhdr->path); 
			else
				sprintf(buffer, "%s%s", arg3, fhdr->path); 

 			/* CHFLAGS */
 			/* Clear flags first */
			retcode = pakman_chflags(buffer, 0);
			/* MKDIR */
			retcode = pakman_rmkdir(
				buffer, 8888, 99999, 99999
			);
			if(retcode != PAKMAN_ERROR_NOERROR)
			{
				close(fd_in);
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}

			/* CHMOD */
			retcode = pakman_chmod(buffer, fhdr->finfo.st_mode);
			if(retcode != PAKMAN_ERROR_NOERROR)
			{
				close(fd_in);
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}

			/* CHOWN */
			retcode = pakman_chown(
				buffer, fhdr->finfo.st_uid, fhdr->finfo.st_gid
			);
			if(retcode != PAKMAN_ERROR_NOERROR)
			{
				close(fd_in);
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}

			/* UTIMES */
			retcode = pakman_utimes(
				buffer,
				fhdr->finfo.st_atime,
				fhdr->finfo.st_mtime,
				fhdr->finfo.st_ctime
			);

 			/* CHFLAGS */
#ifndef	LINUX
			retcode = pakman_chflags(buffer, fhdr->finfo.st_flags);
			if(retcode != PAKMAN_ERROR_NOERROR)
			{
				close(fd_in);
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}
#endif

 		  	/* Store dir name */
			retcode = pakman_db_set(
				db, fhdr->path, strlen(fhdr->path) + 1,
				"PKG_%s_DIR_%ld", hdr->name, instdirs
			);
		   if(retcode != PAKMAN_ERROR_NOERROR)
		   {
				retcode = PAKMAN_ERROR_DBERROR;
				goto cmd_install_end;
		   }
		  	/* Store FINFO */
			retcode = pakman_db_set(
				db, (char *)&fhdr->finfo, sizeof(fhdr->finfo),
				"PKG_%s_DIR_%ld_FINFO", hdr->name, instdirs
			);
		   if(retcode != PAKMAN_ERROR_NOERROR)
		   {
				retcode = PAKMAN_ERROR_DBERROR;
				goto cmd_install_end;
		   }
			instdirs++;
		}
	   fhdr = (pakman_fileinfo_t *)tmpp;
	}
   /* Traverse files. */
   tmpp = filebuf;
   tmpp += sizeof(pakman_header_t);
   fhdr = (pakman_fileinfo_t *)tmpp;
	fprintf(
		stdout, "%s: %p %p %p\n",
		__FUNCTION__, filebuf, (void *)fhdr, filebuf_end
	);
	while((char *)fhdr < (char *)filebuf_end)
	{
		/* Neeeeeeeeeeeext!! */
	   tmpp += sizeof(pakman_fileinfo_t);

		if(S_ISDIR(fhdr->finfo.st_mode))
		{
		   fhdr = (pakman_fileinfo_t *)tmpp;
			continue;
		}
		/* Print file info. */
		if(!arg3)
			sprintf(buffer, "%s", fhdr->path); 
		else
			sprintf(buffer, "%s%s", arg3, fhdr->path); 

#ifndef	LINUX
		flagsstr = fflagstostr(fhdr->finfo.st_flags);
#endif
		fprintf(
			stdout, "%s_file: %s, sha: %s, size: %ld, mode: %o, ownership: %ld:%ld, flags: %s\n",
			__FUNCTION__, fhdr->path, fhdr->sha,
			(long)fhdr->finfo.st_size, fhdr->finfo.st_mode, 
			(long)fhdr->finfo.st_uid, (long)fhdr->finfo.st_gid,
			flagsstr == NULL ? "XXXX" : flagsstr
		);
		if(flagsstr != NULL)
			free(flagsstr);

		/* CHFLAGS */
		/* Clear flags first */
		retcode = pakman_chflags(buffer, 0);
		if(S_ISBLK(fhdr->finfo.st_mode)) {
			if(mknod(buffer, fhdr->finfo.st_mode, fhdr->finfo.st_rdev))
			{
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}
		} else if(S_ISCHR(fhdr->finfo.st_mode)) {
			if(mknod(buffer, fhdr->finfo.st_mode, fhdr->finfo.st_rdev))
			{
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}
		} else if(S_ISFIFO(fhdr->finfo.st_mode)) {
			if(mkfifo(buffer, fhdr->finfo.st_mode))
			{
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}
		} else if(S_ISLNK(fhdr->finfo.st_mode)) {
			symlink(tmpp, buffer);
			tmpp += strlen(tmpp) + 1;
		} else if(S_ISREG(fhdr->finfo.st_mode)) {
			sprintf(tmpfilename, "%s/pakXXXXXX", dirname(buffer));
			fd_out = mkstemp(tmpfilename);
			if(fd_out < 0)
			{
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}
			if(
				write(
					fd_out, tmpp,
					fhdr->finfo.st_size
				) < fhdr->finfo.st_size
			)
			{
				unlink(tmpfilename);
				retcode = PAKMAN_ERROR_OSERROR;
				goto cmd_install_end;
			}
			close(fd_out);
			/* Verify SHA sum. */
			SHA_File(tmpfilename, buffer2);
			if(!force && strcmp(fhdr->sha, buffer2))
			{
				fprintf(stdout, "%s: SHA %s - %s\n", __FUNCTION__, buffer, buffer2);
				retcode = PAKMAN_ERROR_SHAFAILS;
				goto cmd_install_end;
			} else 
				fprintf(stdout, "%s: SHA OK.\n", __FUNCTION__);
				if(rename(tmpfilename, buffer) < 0)
				{
					retcode = PAKMAN_ERROR_OSERROR;
					goto cmd_install_end;
				}
			/* Update pointer. */
			tmpp += fhdr->finfo.st_size;
		} else if(S_ISSOCK(fhdr->finfo.st_mode)) {
		} else if(S_ISWHT(fhdr->finfo.st_mode)) {
		}
		/* CHMOD */
		retcode = pakman_chmod(buffer, fhdr->finfo.st_mode);
		if(retcode != PAKMAN_ERROR_NOERROR)
		{
			retcode = PAKMAN_ERROR_OSERROR;
			goto cmd_install_end;
		}
		/* CHOWN */
		retcode = pakman_chown(
			buffer, fhdr->finfo.st_uid, fhdr->finfo.st_gid
		);
		if(retcode != PAKMAN_ERROR_NOERROR)
		{
			retcode = PAKMAN_ERROR_OSERROR;
			goto cmd_install_end;
		}
		/* UTIMES */
		retcode = pakman_utimes(
			buffer,
			fhdr->finfo.st_atime,
			fhdr->finfo.st_mtime,
			fhdr->finfo.st_ctime
		);
		if(retcode != PAKMAN_ERROR_NOERROR)
		{
			retcode = PAKMAN_ERROR_OSERROR;
			goto cmd_install_end;
		}
#ifndef	LINUX
		/* CHFLAGS */
		retcode = pakman_chflags(buffer, fhdr->finfo.st_flags);
		if(retcode != PAKMAN_ERROR_NOERROR)
		{
			close(fd_in);
			retcode = PAKMAN_ERROR_OSERROR;
			goto cmd_install_end;
		}
#endif

	  	/* Store file! name */
		retcode = pakman_db_set(
			db, fhdr->path, strlen(fhdr->path) + 1,
			"PKG_%s_FILE_%ld", hdr->name, instfiles
		);
	   if(retcode != PAKMAN_ERROR_NOERROR)
	   {
			retcode = PAKMAN_ERROR_DBERROR;
			goto cmd_install_end;
	   }
	  	/* Store SHA */
		retcode = pakman_db_set(
			db, fhdr->sha, strlen(fhdr->sha) + 1,
			"PKG_%s_FILE_%ld_SHA", hdr->name, instfiles
		);
	   if(retcode != PAKMAN_ERROR_NOERROR)
	   {
			retcode = PAKMAN_ERROR_DBERROR;
			goto cmd_install_end;
	   }
	  	/* Store FINFO */
		retcode = pakman_db_set(
			db, (char *)&fhdr->finfo, sizeof(fhdr->finfo),
			"PKG_%s_FILE_%ld_FINFO", hdr->name, instfiles
		);
	   if(retcode != PAKMAN_ERROR_NOERROR)
	   {
			retcode = PAKMAN_ERROR_DBERROR;
			goto cmd_install_end;
	   }
		instfiles++;
	   fhdr = (pakman_fileinfo_t *)tmpp;
	}
	/* Done ;) */
	fprintf(stdout,
		"%s: Installed: %ld files, %ld directories.\n",
		__FUNCTION__, instfiles, instdirs
	);
	
	/* Register package! */
	retcode = pakman_db_set(
		db, hdr->version, strlen(hdr->version) + 1, "PKG_%s_VERSION", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
	retcode = pakman_db_set(
		db, hdr->arch, strlen(hdr->arch) + 1, "PKG_%s_ARCH", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
	retcode = pakman_db_set(
		db, hdr->description, strlen(hdr->description) + 1,
		"PKG_%s_DESCRIPTION", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
  	/* Store package name and install date. */
  	time(&mytime);
	sprintf(buffer, "%ld", (long)mytime);
	retcode = pakman_db_set(
		db, buffer, strlen(buffer) + 1,
		"_PKG_%s", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }

  	/* Store package files and dirs totals. */
	sprintf(buffer, "%ld", instfiles);
	retcode = pakman_db_set(
		db, buffer, strlen(buffer) + 1,
		"PKG_%s_FILES", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
	sprintf(buffer, "%ld", instdirs);
	retcode = pakman_db_set(
		db, buffer, strlen(buffer) + 1,
		"PKG_%s_DIRS", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
	sprintf(buffer, "%ld", (long)hdr->time);
	retcode = pakman_db_set(
		db, buffer, strlen(buffer) + 1,
		"PKG_%s_RDATE", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
	retcode = pakman_db_set(
		db, hdr->status1, strlen(hdr->status1) + 1,
		"PKG_%s_STATUS1", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }
	retcode = pakman_db_set(
		db, hdr->depends, strlen(hdr->depends) + 1,
		"PKG_%s_DEPENDS", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }

	retcode = pakman_db_set(
		db, (char *)pakman_pkg_strstatus(PAKMAN_PKG_INSTALLED),
		strlen(pakman_pkg_strstatus(PAKMAN_PKG_INSTALLED)) + 1,
		"PKG_%s_STATUS2", hdr->name
	);
   if(retcode != PAKMAN_ERROR_NOERROR)
   {
		retcode = PAKMAN_ERROR_DBERROR;
		goto cmd_install_end;
   }

	fprintf(
		stdout, "%s: Successfully registered package: %s.\n",
		__FUNCTION__, hdr->name
	);

   /* Unmap File. */
   munmap(filebuf, finfo.st_size);

   /* Close package file. */
   close(fd_in);
   fd_in = -1;

	/* Done. */
cmd_install_end:
	if(retcode != PAKMAN_ERROR_NOERROR)
		if(pakman_exists(tmpfilename) != PAKMAN_ERROR_FILENEXISTS)
			pakman_rm(tmpfilename);
	if((retcode == PAKMAN_ERROR_NARGS) || (retcode == PAKMAN_ERROR_INVARGS))
		fprintf(
			stdout, "%s: use: <filename> [force|noforce] [prefix] [pakman_db path]\n",
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
