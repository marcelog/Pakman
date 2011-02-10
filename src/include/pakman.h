/*****************************************************************************
 *
 * pakman.h: Main config file.
 * --------
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
#ifndef	_PAKMAN_H
#define	_PAKMAN_H

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	<stdio.h>
#include	<time.h>
#include	<sys/stat.h>
#include	<unistd.h>

/*****************************************************************************
 * Types and Definitions.
 *****************************************************************************/
typedef enum
{
	PAKMAN_FILE_ERROR,
	PAKMAN_FILE_REGULAR,
	PAKMAN_FILE_DIRECTORY,
	PAKMAN_FILE_SYMBOLIC,
	PAKMAN_FILE_SOCKET,
	PAKMAN_FILE_BLOCK,
	PAKMAN_FILE_CHAR,
	PAKMAN_FILE_WHITEOUT,
	PAKMAN_FILE_FIFO,
	PAKMAN_FILE_UNKNOWN
} pakman_file_type_t;

typedef struct
{
	char path[1024];
	char sha[64];
	struct stat finfo;
} pakman_fileinfo_t;

typedef struct
{
	char name[128];
	time_t time;
	time_t time2;
	long files;
	long directories;
	char version[16];
	char arch[8];
	char description[1024];
	char prefix[FILENAME_MAX];
	char status1[64];
	char status2[64];
	char depends[1024];
} pakman_header_t;

/*****************************************************************************
 * Constants.
 *****************************************************************************/
#define	PAKMAN_VERSION_MAJOR	1
#define	PAKMAN_VERSION_MINOR	0
#define	PAKMAN_VERSION_MICRO	1

#define	PAKMAN_TMP		"/tmp"
#define	PAKMAN_DB_PATH	"/var/pakman/db"
#define	PAKMAN_DB_FILE	"main.db"

#define	PAKMAN_PKG_REPOS	"/var/pakman/distfiles"

#define	PAKMAN_DB_PATH_MODE	S_IRWXU
#define	PAKMAN_DB_FILE_MODE	S_IRUSR | S_IWUSR
#define	PAKMAN_TMP_FILE_MODE	S_IRUSR | S_IWUSR

#define	PAKMAN_DB_UID	0
#define	PAKMAN_DB_GID	0

#define	COPYRIGHT	"COPYRIGHT(C) Marcelo Gornstein"

char *PAKMAN_DB_FILEPATH;

#ifdef LINUX
#define S_IFWHT 0160000
#define S_ISWHT(m) (((m) & S_IFMT) == S_IFWHT)
#endif

#endif
