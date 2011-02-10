/*****************************************************************************
 *
 * cmd.c: Cmd definitions.
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

/*****************************************************************************
 * Includes.
 *****************************************************************************/
#include	"include/cmd.h"

/*****************************************************************************
 * Constants.
 *****************************************************************************/

/* Command function pointers. */
static const pakman_cmd_handler_t CMD_HANDLERS[PAKMAN_CMD_DUMMY2] = {
	cmd_na,
	cmd_show,
	cmd_query,
	cmd_search,
	cmd_install,
	cmd_deinstall,
	cmd_verify,
	cmd_na,
	cmd_createdb,
	cmd_create,
	cmd_na
};

/* Command names. */
static const char *COMMANDS_STR[PAKMAN_CMD_DUMMY2] = {
	"NULL",
	"show",
	"query",
	"search",
	"install",
	"deinstall",
	"verify",
	"help",
	"createdb",
	"create",
	"update"
};

/* Command descriptions. */
static const char *COMMANDS_DESCR[PAKMAN_CMD_DUMMY2] = {
	"NULL",
	"shows information about pakman",
	"shows information about packages",
	"search packages",
	"installs packages",
	"deinstalls packages",
	"verifies packages installation",
	"shows detailed help about commands",
	"creates/initialize/reset packages database",
	"creates a package",
	"updates a package"
};

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/

/* Find the name string for the given command. NULL on error. */
const char *
pakman_strcmd(pakman_cmd_t c)
{
	if((c > PAKMAN_CMD_DUMMY1) && (c < PAKMAN_CMD_DUMMY2))
		return COMMANDS_STR[c];
	else
		return "NULL. Not a command?";
}

/* Find the description string for the given command. NULL on error. */
const char *
pakman_strcmddesc(pakman_cmd_t c)
{
	if((c > PAKMAN_CMD_DUMMY1) && (c < PAKMAN_CMD_DUMMY2))
		return COMMANDS_DESCR[c];
	else
		return "NULL. Not a command?";
}

/* Find the function pointer for the given command. NULL on error. */
pakman_cmd_handler_t 
pakman_cmdhandler(pakman_cmd_t c)
{
	if((c > PAKMAN_CMD_DUMMY1) && (c < PAKMAN_CMD_DUMMY2))
		return CMD_HANDLERS[c];
	else
		return NULL;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
