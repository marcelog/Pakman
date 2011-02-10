/*****************************************************************************
 *
 * file.h: File utilities.
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
#ifndef	_PAKMAN_FILE_H
#define	_PAKMAN_FILE_H

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	"error.h"

/*****************************************************************************
 * External definitions.
 *****************************************************************************/
extern pakman_error_t pakman_mkdir(
	const char *path, mode_t mode, uid_t uid, gid_t gid
);

extern pakman_error_t pakman_rmkdir(
	const char *path, mode_t mode, uid_t uid, gid_t gid
);

extern pakman_error_t pakman_rmdir(const char *path);
extern pakman_error_t pakman_chmod(const char *path, mode_t mode);
extern pakman_error_t pakman_chown(const char *path, uid_t uid, gid_t gid);
extern pakman_error_t pakman_chgrp(const char *path, gid_t uid);
extern pakman_error_t pakman_exists(const char *path);
extern pakman_error_t pakman_rm(const char *path);
extern pakman_error_t pakman_chflags(const char *path, int flags);
/*extern void pakman_tmpname(char *pfx, char *target);*/
extern pakman_error_t pakman_utimes(
	const char *path, time_t a, time_t m, time_t c
);
/*
extern pakman_error_t pakman_file_exists(const char *path);
extern pakman_error_t pakman_dir_exists(const char *path);
*/
#ifdef	LINUX
extern char *SHA_File(const char *filename, char *buf);
#endif

#endif
