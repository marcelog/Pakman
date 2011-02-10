/*****************************************************************************
 *
 * cmd.h: Commands.
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
#ifndef	_PAKMAN_CMD_H
#define	_PAKMAN_CMD_H

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	<stdlib.h>
#include	"error.h"
#include	"db.h"

/*****************************************************************************
 * Types and definitions.
 *****************************************************************************/

/* Command list. */
typedef enum
{
	PAKMAN_CMD_DUMMY1,
	PAKMAN_CMD_SHOW,
	PAKMAN_CMD_QUERY,
	PAKMAN_CMD_SEARCH,
	PAKMAN_CMD_INSTALL,
	PAKMAN_CMD_DEINSTALL,
	PAKMAN_CMD_VERIFY,
	PAKMAN_CMD_CREATEDB,
	PAKMAN_CMD_HELP,
	PAKMAN_CMD_CREATE,
	PAKMAN_CMD_UPDATE,
	PAKMAN_CMD_DUMMY2
} pakman_cmd_t;

typedef pakman_error_t (*pakman_cmd_handler_t)(
	int argc, char *argv[], pakman_db_t db
);

/*****************************************************************************
 * External definitions.
 *****************************************************************************/
extern pakman_error_t cmd_create(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_createdb(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_deinstall(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_install(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_na(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_query(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_search(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_show(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_help(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_verify(int argc, char *argv[], pakman_db_t db);
extern pakman_error_t cmd_update(int argc, char *argv[], pakman_db_t db);

extern const char *pakman_strcmd(pakman_cmd_t c);
extern const char *pakman_strcmddesc(pakman_cmd_t c);
extern pakman_cmd_handler_t pakman_cmdhandler(pakman_cmd_t c);

#endif
