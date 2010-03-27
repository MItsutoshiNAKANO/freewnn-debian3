/*
 * $Id: localalloc.c,v 1.2 2001/06/14 18:16:16 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Author: OMRON SOFTWARE Co., Ltd. <freewnn@rd.kyoto.omronsoft.co.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Commentary:
 *
 * Change log:
 *      '99/04/01       本間　亮 <honma@nasu-net.or.jp>
 *              size を sizeof(char *) の整数倍に切り上げる。
 *
 * Last modified date: 20,Mar.1999
 *
 * Code:
 *
 */

/*
 * memory allocation routines
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */

extern char *malloc (), *realloc (), *calloc ();
#ifdef ALLOC_DEBUG
#define DEBUG_TBL_SIZE  10000
typedef struct _alloc_debug_struct
{
  char *ptr;
  int size;
}
debug_tbl;

debug_tbl alloc_tbl[DEBUG_TBL_SIZE];
static int debug_initialized = 0;

void
initialize_debug ()
{
  register int i;

  for (i = 0; i < DEBUG_TBL_SIZE; i++)
    {
      alloc_tbl[i].ptr = NULL;
      alloc_tbl[i].size = 0;
    }
  debug_initialized = 1;
}

static void
register_debug (ptr, size)
     char *ptr;
     int size;
{
  register int i;

  for (i = 0; i < DEBUG_TBL_SIZE; i++)
    {
      if (alloc_tbl[i].ptr == NULL)
        {
          alloc_tbl[i].ptr = ptr;
          alloc_tbl[i].size = size;
          return;
        }
    }
  print_out ("alloc_table over flow");
}

void
unregister_debug (ptr)
     char *ptr;
{
  register int i;

  for (i = 0; i < DEBUG_TBL_SIZE; i++)
    {
      if (alloc_tbl[i].ptr == ptr)
        {
          alloc_tbl[i].ptr = NULL;
          alloc_tbl[i].size = 0;
          return;
        }
    }
  print_out1 ("illegal calling of free ptr = %x", ptr);
}

#endif

char *
Malloc (size)
     unsigned size;
{
  char *ptr;
  if (size == 0)
    return (NULL);
  size += (sizeof (char *) - (size % sizeof (char *)));
  if ((ptr = malloc (size)) == NULL)
    {
#ifdef ALLOC_DEBUG
      print_out1 ("alloc failed with size = %d", size);
#endif
      return (NULL);
    }
#ifdef ALLOC_DEBUG
  if (!debug_initialized)
    {
      initialize_debug ();
    }
  register_debug (ptr, size);
#endif
  return (ptr);
}

#ifdef nodef                    /* should not use Realloc, because realloc may broke
                                   old data if it fails allocation of new area */
char *
Realloc (ptr, size)
     char *ptr;
     unsigned size;
{
  if (size == 0)
    return (ptr);
  size += (sizeof (char *) - (size % sizeof (char *)));
  if (ptr == NULL)
    return (Malloc (size));
#ifdef ALLOC_DEBUG
  unregister_debug (ptr);
#endif
  if ((ptr = realloc (ptr, size)) == NULL)
    {
      return (NULL);
    }
#ifdef ALLOC_DEBUG
  register_debug (ptr, size);
#endif
  return (ptr);
}
#endif

char *
Calloc (num, size)
     unsigned num, size;
{
  char *ptr;
  if (size == 0)
    return (NULL);
  size += (sizeof (char *) - (size % sizeof (char *)));
  if ((ptr = calloc (num, size)) == NULL)
    {
      return (NULL);
    }
#ifdef ALLOC_DEBUG
  if (!debug_initialized)
    {
      initialize_debug ();
    }
  register_debug (ptr, size);
#endif
  return (ptr);
}

void
Free (ptr)
     char *ptr;
{
  if (ptr != NULL)
    free (ptr);
#ifdef ALLOC_DEBUG
  unregister_debug (ptr);
#endif
}


char *
alloc_and_copy (src)
     register char *src;
{
  register char *ptr;
  register unsigned int n;

  if (src == NULL)
    return (NULL);
  if ((ptr = Malloc ((n = strlen (src)) + 1)) == NULL)
    {
      malloc_error ("allocation of work area");
      return (NULL);
    }
  bcopy (src, ptr, n);
  ptr[n] = '\0';
  return (ptr);
}
