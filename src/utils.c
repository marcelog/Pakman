/*****************************************************************************
 *
 * utils.c: Misc Utilities.
 * -------
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
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	"include/utils.h" 
 
/*****************************************************************************
 * Constants.
 *****************************************************************************/

/*****************************************************************************
 * Code STARTS.
 *****************************************************************************/

/* Allocates and zero memory. */
void *
pakman_malloc(long size)
{
	void *b = malloc(size);
	if(b)
		memset(b, 0, size);
	return b;
}

/* Free. */
void
pakman_free(void *b)
{
	if(b)
		free(b);
}
/*****************************************************************************
 * Code ENDS.
 *****************************************************************************/
