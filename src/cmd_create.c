/*****************************************************************************
 *
 * cmd_create.c: Handles Package creation.
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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<dirent.h>
#include	<time.h>
#include	<fcntl.h>
#include	<sys/mman.h>
#ifdef LINUX
#include	<openssl/sha.h>
#else
#include	<sha.h>
#endif
#include	<libgen.h>
#include	"include/pakman.h"
#include	"include/utils.h"
#include	"include/file.h"
#include	"include/cmd.h"
#include	"include/db.h"
#include	"include/error.h"

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
 
/*
 * Recursive function. Will add all files except directories and will call
 * itself for directories.
 */
static pakman_error_t
archive_dir(
	const char *path, char *dst, int fd_out, long *files, long *directories,
	int first
)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	DIR *dir = NULL;
	struct dirent *dir_ent = NULL;
	pakman_fileinfo_t fhdr;
	char file_path[FILENAME_MAX];
	int fd_in = -1;
	char *filebuf = NULL;
	char buffer[FILENAME_MAX];
	char buffer2[FILENAME_MAX];
	char *flagsstr = NULL;

	if(first)
	{
		(*directories)++;
		memset(fhdr.sha, 0, sizeof(fhdr.sha));
		sprintf(fhdr.path, "%s", dst);
		/* Fill Stat. */
		if(lstat(path, &fhdr.finfo))
		{
			retcode = PAKMAN_ERROR_OSERROR;
			goto archive_dir_end;
		}
		/* Write File header. */
		if(write(
			fd_out, (void *)&fhdr,
			sizeof(pakman_fileinfo_t)) < sizeof(pakman_fileinfo_t)
		)
		{
			retcode = PAKMAN_ERROR_OSERROR;
			goto archive_dir_end;
		}
	}
	/* Open directory. */
	dir = opendir(path);
	if(!dir)
	{
		retcode = PAKMAN_ERROR_OSERROR;
		goto archive_dir_end;
	}
	/* Gather filelist. */
	while((dir_ent = readdir(dir)))
	{
		if(!strcmp(dir_ent->d_name, "."))
			continue;
		if(!strcmp(dir_ent->d_name, ".."))
			continue;
		/* Fill name. */
		if(strcmp(dst, "/")) {
			sprintf(fhdr.path, "%s/%s", dst, dir_ent->d_name);
			sprintf(file_path, "%s/%s", path, dir_ent->d_name);
		} else {
			sprintf(fhdr.path, "/%s", dir_ent->d_name);
			sprintf(file_path, "/%s", dir_ent->d_name);
		}
			sprintf(file_path, "%s/%s", path, dir_ent->d_name);
		memset(fhdr.sha, 0, sizeof(fhdr.sha));
		/* Fill Stat. */
		if(lstat(file_path, &fhdr.finfo))
		{
			closedir(dir);
			retcode = PAKMAN_ERROR_OSERROR;
			goto archive_dir_end;
		}
		if(S_ISSOCK(fhdr.finfo.st_mode) || S_ISWHT(fhdr.finfo.st_mode))
		{
			fprintf(
				stdout, "%s: Excluding sock/wht %s\n",
				__FUNCTION__, file_path
			);
			continue;
		}
		if(S_ISREG(fhdr.finfo.st_mode))
		{
			/* SHA. */
			if(SHA_File(file_path, fhdr.sha) == NULL)
			{
				closedir(dir);
				retcode = PAKMAN_ERROR_OSERROR;
				goto archive_dir_end;
			}
		}
		/* Write File header. */
		if(write(
			fd_out, (void *)&fhdr,
			sizeof(pakman_fileinfo_t)) < sizeof(pakman_fileinfo_t)
		)
		{
			closedir(dir);
			retcode = PAKMAN_ERROR_OSERROR;
			goto archive_dir_end;
		}
		/* Show info. */
#ifndef LINUX
		flagsstr = fflagstostr(fhdr.finfo.st_flags);
#endif
		fprintf(
			stdout, "%s: Adding %s:%s:%lu:%d:%d:%s\n",
			__FUNCTION__, fhdr.path, fhdr.sha,
			(unsigned long)fhdr.finfo.st_size,
			fhdr.finfo.st_uid, fhdr.finfo.st_gid,
			flagsstr == NULL ? "XXXX" : flagsstr
		);
		if(flagsstr != NULL)
			free(flagsstr);
		/* Look for dir or file. */
		if(dir_ent->d_type != DT_DIR)
		{
			if(S_ISLNK(fhdr.finfo.st_mode))
			{
				memset(buffer, 0, sizeof(buffer));
				if(readlink(file_path, buffer, sizeof(buffer)) < 0)
				{
					close(fd_in);
					closedir(dir);
					retcode = PAKMAN_ERROR_OSERROR;
					goto archive_dir_end;
				}
				fprintf(stdout, "%s: Link: %s -> %s\n", __FUNCTION__, file_path, buffer);
				if(write(
					fd_out, (void *)buffer,
					strlen(buffer) + 1) < strlen(buffer) + 1
				)
				{
					close(fd_in);
					closedir(dir);
					retcode = PAKMAN_ERROR_OSERROR;
					goto archive_dir_end;
				}
			} else if(S_ISREG(fhdr.finfo.st_mode)) {
				/* Open file. */
				fd_in = open(file_path, O_RDONLY);
				if(fd_in < 0)
				{
					closedir(dir);
					retcode = PAKMAN_ERROR_OSERROR;
					goto archive_dir_end;
				}
				/* Map file. */
				filebuf = mmap(
					NULL, fhdr.finfo.st_size, PROT_READ,
					MAP_SHARED, fd_in, 0
				);
				if(!filebuf)
				{
					close(fd_in);
					closedir(dir);
					retcode = PAKMAN_ERROR_OSERROR;
					goto archive_dir_end;
				}
				/* Write File Data. */
				if(write(
					fd_out, (void *)filebuf,
					fhdr.finfo.st_size) < fhdr.finfo.st_size
				)
				{
					close(fd_in);
					closedir(dir);
					retcode = PAKMAN_ERROR_OSERROR;
					goto archive_dir_end;
				}
				/* Unmap file. */
				munmap(filebuf, fhdr.finfo.st_size);
				/* Close file. */
				close(fd_in);
			}
			(*files)++;
		} else {
			(*directories)++;
			sprintf(buffer2, "%s/%s", dst, basename(file_path));
			retcode = archive_dir(file_path, buffer2, fd_out, files, directories, 0);
			if(retcode != PAKMAN_ERROR_NOERROR)
			{
				closedir(dir);
				goto archive_dir_end;
			}
			
		}
		/* Show info. */
	}
	/* Close this directory. */
	closedir(dir);
archive_dir_end:
	return retcode;
}

pakman_error_t
cmd_create(int argc, char *argv[], pakman_db_t db)
{
	char *pkgname;
	char *src;
	char *pfx;
	char *arch;
	char *version;
	char *description;
	char *depends;
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	pakman_header_t hdr;
	int fd_out = -1;
	char file_tmp_name[FILENAME_MAX];

	memset(file_tmp_name, 0, sizeof(file_tmp_name));
	/* Check number of arguments. */
	fprintf(stdout, "%s: Start.\n", __FUNCTION__);
	if(argc < 7)
	{
		fprintf(stdout, "use: <pkgname> <src> <target_pfx> <version> <arch> <description> <depends>\n");
		retcode = PAKMAN_ERROR_NARGS;
		goto cmd_create_end;
	}

	/* Get arguments. */
	pkgname = argv[0];
	src = argv[1];
	pfx = argv[2];
	version = argv[3];
	arch = argv[4];
	description = argv[5];
	depends = argv[6];
	
	memset((void *)&hdr, 0, sizeof(hdr));

	/* Check src. */
	if(pakman_exists(src) != PAKMAN_FILE_DIRECTORY)
	{
		fprintf(
			stdout, "%s: %s does not exist or is not a directory.\n",
			__FUNCTION__, src
		);
		retcode = PAKMAN_ERROR_NOTDIR;
		goto cmd_create_end;
	}

	/* Fill Preliminary Header Info. */
	time(&hdr.time);
	strncat(hdr.name, pkgname, sizeof(hdr.name));
	strncat(hdr.prefix, pfx, sizeof(hdr.prefix));
	strncat(hdr.version, version, sizeof(hdr.version));
	strncat(hdr.arch, arch, sizeof(hdr.arch));
	strncat(hdr.description, description, sizeof(hdr.description));
	strncat(hdr.depends, depends, sizeof(hdr.depends));
	
	fprintf(
		stdout, "%s: %s -> \"%s\" to be installed in %s\n",
		__FUNCTION__, src, hdr.name, hdr.prefix
	); 

	/* Open tmp file. */
	sprintf(file_tmp_name, "%s-%s-%s.pak", pkgname, version, arch);
	fprintf(stdout, "%s: Creating %s\n", __FUNCTION__, file_tmp_name);
	if(pakman_exists(file_tmp_name) != PAKMAN_ERROR_FILENEXISTS)
	{
		fprintf(
			stdout,
			"%s: Target package file already exists. Please remove it and try again.\n",
			__FUNCTION__
		);
		retcode = PAKMAN_ERROR_FILEEXISTS;
		goto cmd_create_end;
	}
	fprintf(stdout, "%s: Opening %s.\n", __FUNCTION__, file_tmp_name);
	fd_out = open(file_tmp_name, O_RDWR | O_CREAT, PAKMAN_TMP_FILE_MODE);
	if(fd_out < 0)
	{
		retcode = PAKMAN_ERROR_OSERROR;
		goto cmd_create_end;
	}
	/* Write header. */
	if(write(fd_out, (void *)&hdr, sizeof(hdr)) < sizeof(hdr))
	{
		retcode = PAKMAN_ERROR_OSERROR;
		goto cmd_create_end;
	}

	/* Recursively archive source dir. */
	hdr.files = 0;
	hdr.directories = 0;
	retcode = archive_dir(src, pfx, fd_out, &hdr.files, &hdr.directories, 1);
	fprintf(
		stdout, "%s: %ld files | %ld directories\n",
		__FUNCTION__, hdr.files, hdr.directories
	);
	if(lseek(fd_out, 0, SEEK_SET))
	{
		retcode = PAKMAN_ERROR_OSERROR;
		goto cmd_create_end;
	}
	/* Write header. */
	if(write(fd_out, (void *)&hdr, sizeof(hdr)) < sizeof(hdr))
	{
		retcode = PAKMAN_ERROR_OSERROR;
		goto cmd_create_end;
	}
	close(fd_out);

/* Done. */
cmd_create_end:
	if(fd_out > 0)
	{
		close(fd_out);
	}
	/* Cleanup ? */
	if(retcode != PAKMAN_ERROR_NOERROR)
	{
		fprintf(stdout, "%s: Cleaning up.\n", __FUNCTION__);
		if(retcode != PAKMAN_ERROR_FILEEXISTS)
			if(strlen(file_tmp_name))
				pakman_rm(file_tmp_name);
	}
	fprintf(stdout, "%s: End.\n", __FUNCTION__);
	return retcode;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
