/*****************************************************************************
 *
 * main.c: Main command for package manager.
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
 * Version: 0.0.1
 * -------
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
#include	<unistd.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<string.h>
#include	<errno.h>

#include	"include/pakman.h"
#include	"include/cmd.h"
#include	"include/utils.h"
#include	"include/error.h"

/*****************************************************************************
 * Forward declarations.
 *****************************************************************************/
static void showhelp(void);

/*****************************************************************************
 * Constants.
 *****************************************************************************/

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/

/*
 * Show usage.
 */
void
showhelp(void)
{
	pakman_cmd_t i;
	
	fprintf(stdout, "\nPAKMAN Help:\n\n");
	for(i = 1; i < PAKMAN_CMD_DUMMY2; i++)
		fprintf(stdout, "\t%s\t\t%s\n", pakman_strcmd(i), pakman_strcmddesc(i));
	fprintf(stdout, "\n\n");
}

/*
 * Main Entry point.
 */
int
main(int argc, char *argv[])
{
	pakman_db_t db = NULL;
	pakman_cmd_t i = PAKMAN_CMD_DUMMY1;
	int cmdok = 0;
	pakman_error_t cmd_result = PAKMAN_ERROR_DUMMY1;
	/* Resolve DB. */
	PAKMAN_DB_FILEPATH =
		(char *)pakman_malloc(
			strlen(PAKMAN_DB_PATH) + 2 + strlen(PAKMAN_DB_FILE)
		);
	if(!PAKMAN_DB_FILEPATH)
	{
		fprintf(
			stdout, "%s: XXX FATAL ERROR, CANT MALLOC. XXX", __FUNCTION__
		);
		exit(255);
	}
	sprintf(PAKMAN_DB_FILEPATH, "%s/%s", PAKMAN_DB_PATH, PAKMAN_DB_FILE);

	/* Show initial message. */
	fprintf(
		stdout, "%s: PAKMAN - Package Manager - %d.%d.%d using %s\n",
		__FUNCTION__,
		PAKMAN_VERSION_MAJOR,
		PAKMAN_VERSION_MINOR,
		PAKMAN_VERSION_MICRO,
		PAKMAN_DB_FILEPATH
	);

	/* Check arguments. */
	if(argc < 2)
	{
		showhelp();
		pakman_free(PAKMAN_DB_FILEPATH);
		exit(EXIT_SUCCESS);
	}

	/* Check commands. */
	for(; i < PAKMAN_CMD_DUMMY2; i++)
	{
		if(!strcasecmp(argv[1], pakman_strcmd(i)))
		{
			/* Found valid command. */
			cmdok = 1;
			/* Call Handler. */
			cmd_result = pakman_cmdhandler(i)(argc-2, &argv[2], db);
		}
	}
	/* If no matching command was found. */
	if(!cmdok)
	{
		fprintf(stdout, "INVALID COMMAND.\n");
		showhelp();
	} else {
		/* Check handler result. */
		if(cmd_result == PAKMAN_ERROR_NOERROR)
		{
			fprintf(stdout, "%s: Command OK.\n", __FUNCTION__);
			cmd_result = 0;
		} else {
			fprintf(
				stdout, "%s: Command Exited with error: %d - %s %s\n",
				__FUNCTION__, cmd_result, pakman_strerror(cmd_result),
				cmd_result == PAKMAN_ERROR_OSERROR ? strerror(errno) : ""
			);					
		}
	}

	/* Done. */
	exit(cmd_result);
	return cmd_result;
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
