/*****************************************************************************
 *
 * db.c: Database handling code.
 * ----
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
#include	<gdbm.h>
#include	<stdarg.h>
#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	"include/pakman.h"
#include	"include/db.h" 
#include	"include/error.h" 
 
/*****************************************************************************
 * Forward Declarations.
 *****************************************************************************/
static void gdbm_error_handler(char *str);

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/
/* For gdbm error handling. */
const char *
pakman_db_version(void)
{
	return gdbm_version;
}

static void
gdbm_error_handler(char *str)
{
	if(!str)
		return;

	fprintf(stdout, "%s: %s.\n", __FUNCTION__, str);
	return;
}

pakman_error_t
pakman_db_set(pakman_db_t db, const char *value, int len, const char *key, ...)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	va_list args;
	char buffer1[1024];
	char buffer2[1024];
	datum dat1;
	datum dat2;
	/* Start. */
	if(!db || !value || !key || len < 1)
		return PAKMAN_ERROR_INVARGS;
	memset(buffer1, 0, sizeof(buffer1));
	memset(buffer2, 0, sizeof(buffer2));

   gdbm_errno = GDBM_NO_ERROR;
	va_start(args, key);
	vsnprintf(buffer1, sizeof(buffer1), key, args);
	va_end(args);

   dat1.dptr = buffer1;
   dat1.dsize = strlen(buffer1) + 1;
	dat2.dptr = (char *)value;
	dat2.dsize = len;
	
   /* Store it! */
	gdbm_store(db, dat1, dat2, GDBM_REPLACE);

	/* Done. */
	if(gdbm_errno != GDBM_NO_ERROR)
	{
		retcode = PAKMAN_ERROR_DBERROR;
		fprintf(
			stdout, "%s: gdbm error: %s\n",
			__FUNCTION__, gdbm_strerror(gdbm_errno)
		);
	}
  	return retcode;
}

pakman_error_t
pakman_db_del(pakman_db_t db, const char *key, ...)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	va_list args;
	char buffer1[1024];
	datum dat1;

	/* Start. */
	if(!db || !key)
		return PAKMAN_ERROR_INVARGS;
	memset(buffer1, 0, sizeof(buffer1));

   gdbm_errno = GDBM_NO_ERROR;
	va_start(args, key);
	vsnprintf(buffer1, sizeof(buffer1), key, args);
	va_end(args);

	/* Start. */
   dat1.dptr = buffer1;
   dat1.dsize = strlen(buffer1) + 1;
	gdbm_delete(db, dat1);

	/* Done. */
	if(gdbm_errno != GDBM_NO_ERROR)
	{
		fprintf(
			stdout, "%s: gdbm error: %s\n",
			__FUNCTION__, gdbm_strerror(gdbm_errno)
		);
	}
	return retcode;
}

pakman_error_t
pakman_db_get(pakman_db_t db, char *value, const char *key, ...)
{
	pakman_error_t retcode = PAKMAN_ERROR_NOERROR;
	va_list args;
	char buffer1[1024];
	char buffer2[1024];
	datum dat1;
	datum dat2;

	/* Start. */
	if(!db || !value || !key)
		return PAKMAN_ERROR_INVARGS;
	memset(buffer1, 0, sizeof(buffer1));
	memset(buffer2, 0, sizeof(buffer2));

   gdbm_errno = GDBM_NO_ERROR;
	va_start(args, key);
	vsnprintf(buffer1, sizeof(buffer1), key, args);
	va_end(args);

   dat1.dptr = buffer1;
   dat1.dsize = strlen(buffer1) + 1;
	dat2 = gdbm_fetch(db, dat1);
	if(!dat2.dptr)
  	{
		retcode = PAKMAN_ERROR_KEYNOTFOUND;
	} else {
		memcpy(value, dat2.dptr, dat2.dsize);
		free(dat2.dptr);
	}
	/* Done. */
	return retcode;
}

pakman_db_t
pakman_db_open()
{
   GDBM_FILE dbf;
   gdbm_errno = GDBM_NO_ERROR;
	dbf = gdbm_open(
		PAKMAN_DB_FILEPATH, 8192,
		GDBM_WRITER | GDBM_SYNC, PAKMAN_DB_FILE_MODE, gdbm_error_handler
	);
	if(gdbm_errno != GDBM_NO_ERROR)
	{
		fprintf(
			stdout, "%s: gdbm error: %s\n",
			__FUNCTION__, gdbm_strerror(gdbm_errno)
		);
	}
	return dbf;
}

pakman_db_t
pakman_db_create(const char *pfx)
{
   GDBM_FILE dbf;
   char buffer[1024];
   gdbm_errno = GDBM_NO_ERROR;
	if(pfx != NULL)
		sprintf(buffer, "%s/%s", pfx, PAKMAN_DB_FILEPATH);
	else
		sprintf(buffer, "%s", PAKMAN_DB_FILEPATH);
	dbf = gdbm_open(
		buffer , 8192, GDBM_NEWDB | GDBM_SYNC,
		PAKMAN_DB_FILE_MODE, gdbm_error_handler
	);
	if(gdbm_errno != GDBM_NO_ERROR)
	{
		fprintf(
			stdout, "%s: gdbm error: %s\n",
			__FUNCTION__, gdbm_strerror(gdbm_errno)
		);
	}
	return dbf;
}

void
pakman_db_close(pakman_db_t db)
{
	if(!db)
		return;
   gdbm_errno = GDBM_NO_ERROR;
	gdbm_close(db);
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
