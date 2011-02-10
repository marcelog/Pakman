/*****************************************************************************
 *
 * db.h: DB utilities.
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
#ifndef	_PAKMAN_DB_H
#define	_PAKMAN_DB_H

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<gdbm.h>
#include	"error.h"

/*****************************************************************************
 * Types and definitions.
 *****************************************************************************/
typedef	GDBM_FILE pakman_db_t;

/*****************************************************************************
 * External definitions.
 *****************************************************************************/
extern pakman_db_t pakman_db_create(const char *pfx);
extern pakman_db_t pakman_db_open(void);
extern pakman_error_t pakman_db_get(
	pakman_db_t db, char *value, const char *key, ...
);
extern pakman_error_t pakman_db_set(
	pakman_db_t db, const char *value, int len, const char *key, ...
);
extern const char *pakman_db_version(void);

extern void pakman_db_close(pakman_db_t db);
extern pakman_error_t pakman_db_del(pakman_db_t db, const char *key, ...);
#endif
