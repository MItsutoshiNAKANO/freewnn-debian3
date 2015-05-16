/*
 *  $Id: MALLOC.c $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000
 *
 * Maintainer:  FreeWnn Project   <freewnn@tomo.gr.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include "wnn_malloc.h"
#undef  malloc
#undef  realloc
#undef  free

#ifdef  HONMONO
extern char *malloc ();
extern void free ();
extern char *realloc ();
extern char *calloc ();
#else
extern char *MALLOC ();
extern void free ();
extern char *REALLOC ();
extern char *CALLOC ();
#define malloc(x) MALLOC(x)
#define calloc(x,y) CALLOC(x,y)
#define realloc(x,y) REALLOC(x,y)
#endif

char *
malloc0 (size)
     int size;
{
  test2 ();
  if (size == NULL)
    {
      size = 1;
    }
  return (calloc (size, 1));
}

void
free0 (pter)
     char *pter;
{
  if (pter == NULL)
    return;
  free (pter);
}

char *
realloc0 (pter, size)
     char *pter;
     int size;
{
  if (size == NULL)
    {
      size = 1;
    }
  if (pter == NULL)
    {
      return (malloc (size));
    }
  else
    {
      return (realloc (pter, size));
    }
}

/********************************/
#undef  realloc
#undef  malloc
#undef  calloc

extern char *malloc ();
extern char *calloc ();
static char *malloc_p;

test2 ()
{
  test (malloc (5000));
}

test1 ()
{
  test (malloc_p);
}

test (p)
     int *p;
{
  static int tst = 1;
  static int tst1 = 0;
  int *p0, *p1;
  malloc_p = (char *) p;
  if (tst)
    {
      p1 = p - 1;
      p0 = p1;
      while (1)
        {
          if (tst1)
            {
              printf ("%x: %x\n", p1, *p1);
            }
          p1 = *p1 & 0xfffffffe;
          if (p1 == p || p1 == p0)
            {
              printf ("OK!\n");
              break;
            }
        }
    }
}


char *
MALLOC (cnt)
     int cnt;
{
  char *p;
  printf ("malloc size = %d\n", cnt);
  p = malloc (cnt);
  test (p);
  return (p);
}

char *
CALLOC (cnt, size)
     int cnt;
     int size;
{
  static int tmp = 0;
  char *p;
  printf ("%d : calloc size = %d\n", tmp++, cnt);
  p = calloc (cnt, size);
  test (p);
  return (p);
}


char *
REALLOC (p, cnt)
     int *p;
     int cnt;
{
  extern char *realloc ();
  printf ("realloc size = %d\n", cnt);
  p = realloc (p, cnt);
  test (p);
  return (p);
}
