/*****************************************************************************
 *
 * utils.h: Misc utilities.
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
#ifndef	_PAKMAN_UTILS_H
#define	_PAKMAN_UTILS_H

/*****************************************************************************
 * Includes.
 *****************************************************************************/

/*****************************************************************************
 * External definitions.
 *****************************************************************************/
extern void *pakman_malloc(long size); /* XXX Limited to 4GB Files? */
extern void pakman_free(void *b);

#endif
