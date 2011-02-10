/*****************************************************************************
 *
 * file.c: File utilities.
 * ------
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
#include	<unistd.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/time.h>
#include	<errno.h>
#include	<pwd.h>
#include	<grp.h>
#include	<utime.h>
#include <bzlib.h> 
#include	"include/pakman.h"
#include	"include/error.h"
#include	"include/file.h"

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/

 
/* mkdir. */
pakman_error_t
pakman_mkdir(const char *path, mode_t mode, uid_t uid, gid_t gid)
{
	pakman_error_t r = PAKMAN_ERROR_NOERROR;
	pakman_file_type_t ft;
	struct passwd *uidp;
	struct group *gidp;

	/* Try to create directory. */
	if(!path)
		return PAKMAN_ERROR_INVARGS;
	if(!strcmp(path, "/"))
		return r;
	if(strlen(path) < 1)
		return r;
	fprintf(stdout, "%s: %s...", __FUNCTION__, path);

	/* Check if directory already exists. */
	ft = pakman_exists(path);
	if(ft == PAKMAN_ERROR_FILENEXISTS)
	{
		fprintf(stdout, "Mkdir...");
		if(mkdir(path, mode))
		{
			r = PAKMAN_ERROR_OSERROR;
			goto pakman_mkdir_exit;
		}
	} else {
		/* Already exists, check if its a directory. */
		if(ft == PAKMAN_FILE_DIRECTORY)
		{
			fprintf(stdout, "Skipping mkdir...");
			r = PAKMAN_ERROR_NOERROR;
			goto pakman_mkdir_exit;
		} else {
			r = PAKMAN_ERROR_NOTDIR;
			goto pakman_mkdir_exit;
		}
	}
	/* Get uid/gid info. */
	if(uid != 99999)
	{
		uidp = getpwuid(uid);
		gidp = getgrgid(gid);
		fprintf(stdout, "Chowning to: %s:%s...", uidp->pw_name, gidp->gr_name);
		r = pakman_chown(path, uid, gid); 
		if(r != PAKMAN_ERROR_NOERROR)
			goto pakman_mkdir_exit;
	}
	/* Change access mode. */
	if(mode != 8888)
	{
		fprintf(stdout, "Chmod'ing to: %o...", mode);
		r = pakman_chmod(path, mode);
		if(r != PAKMAN_ERROR_NOERROR)
			goto pakman_mkdir_exit;
	}
	/* Done. */
pakman_mkdir_exit:
	if(r == PAKMAN_ERROR_NOERROR)
		fprintf(stdout, "Done!\n");
	else
		fprintf(stdout, "Could not fully create %s\n", path);
	return r;
}

/* Recurse mkdir. */
pakman_error_t
pakman_rmkdir(const char *path, mode_t mode, uid_t uid, gid_t gid)
{
	char *start;
	char *end;
	char buffer[1024];
	int i;
	pakman_error_t r = PAKMAN_ERROR_NOERROR;
	
	/* Start. */
	if(!path)
		return PAKMAN_ERROR_INVARGS;
	fprintf(stdout, "%s: %s...\n", __FUNCTION__, path);
	start = (char *)path;
	end = start + strlen(start);

	i = 0;
	memset(buffer, 0, sizeof(buffer));
	if(strlen(path) > 1)
	{
		buffer[i] = start[i];
		i = 1;
	}
	do
	{
		for(; (start + i) < end; i++)
		{
			if(start[i] == '/')
			{
				if(strlen(path) > i)
					i++;
				break;
			} else
				buffer[i] = start[i];
		}
		/* Create it! */
		r = pakman_mkdir(buffer, mode, uid, gid);
		buffer[i-1] = '/';
	} while(((start + i) < end) && (r == PAKMAN_ERROR_NOERROR));
	/* Done. */
	return r;
}

/* ChFlags. */
pakman_error_t
pakman_chflags(const char *path, int flags)
{
	if(!path)
		return PAKMAN_ERROR_INVARGS;
#ifndef	LINUX
	if(lchflags(path, flags))
		return PAKMAN_ERROR_OSERROR;
#endif
	return PAKMAN_ERROR_NOERROR;
}

/* Chown. */
pakman_error_t
pakman_chown(const char *path, uid_t uid, gid_t gid)
{
	if(!path)
		return PAKMAN_ERROR_INVARGS;
	if(lchown(path, uid, gid))
		return PAKMAN_ERROR_OSERROR;
	return PAKMAN_ERROR_NOERROR;
}
/* Chmod. */
pakman_error_t
pakman_chmod(const char *path, mode_t mode)
{
	if(!path)
		return PAKMAN_ERROR_INVARGS;
	fprintf(stdout, "%s: %s %o\n", __FUNCTION__, path, mode);
#ifdef	LINUX
	if(chmod(path, mode))
#else
	if(lchmod(path, mode))
#endif
		return PAKMAN_ERROR_OSERROR;
	else
		return PAKMAN_ERROR_NOERROR;
}

/* Removes a directory. */
pakman_error_t
pakman_rmdir(const char *path)
{
	if(!path)
		return PAKMAN_ERROR_INVARGS;
	if(rmdir(path))
		return PAKMAN_ERROR_OSERROR;
	else
		return PAKMAN_ERROR_NOERROR;
}

/* Check for file, returns filetype. */
pakman_error_t
pakman_exists(const char *path)
{
	struct stat sb;
	if(!path)
		return PAKMAN_ERROR_INVARGS;

	if(stat(path, &sb))
		return PAKMAN_ERROR_FILENEXISTS;
		
	if(S_ISBLK(sb.st_mode))
		return PAKMAN_FILE_BLOCK;
	else if(S_ISCHR(sb.st_mode))
		return PAKMAN_FILE_CHAR;
	else if(S_ISDIR(sb.st_mode))
		return PAKMAN_FILE_DIRECTORY;
	else if(S_ISFIFO(sb.st_mode))
		return PAKMAN_FILE_FIFO;
	else if(S_ISLNK(sb.st_mode))
		return PAKMAN_FILE_SYMBOLIC;
	else if(S_ISREG(sb.st_mode))
		return PAKMAN_FILE_REGULAR;
	else if(S_ISSOCK(sb.st_mode))
		return PAKMAN_FILE_SOCKET;
	else if(S_ISWHT(sb.st_mode))
		return PAKMAN_FILE_WHITEOUT;
	return PAKMAN_FILE_UNKNOWN;
}

/* Set file times. */
pakman_error_t
pakman_utimes(const char *path, time_t a, time_t m, time_t c)
{
	struct timeval t[2];
	if(!path)
		return PAKMAN_ERROR_INVARGS;
	t[0].tv_sec = a;
	t[0].tv_usec = 0;
	t[1].tv_sec = m;
	t[1].tv_usec = 0;
	
	if(lutimes(path, (struct timeval *)&t))
		return PAKMAN_ERROR_OSERROR;
	else
		return PAKMAN_ERROR_NOERROR;
}

/* Removes a file. */
pakman_error_t
pakman_rm(const char *path)
{
	if(!path)
		return PAKMAN_ERROR_INVARGS;

	if(unlink(path))
		return PAKMAN_ERROR_OSERROR;
	else
		return PAKMAN_ERROR_NOERROR;
}

/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
 
