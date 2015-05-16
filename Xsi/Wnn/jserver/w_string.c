/*
 *  $Id: w_string.c $
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
#include "commonhd.h"
#include "wnn_string.h"

void
Sreverse (w1, w2)
     register w_char *w1, *w2;
{
  wnn_Sreverse (w1, w2);
}

int
Sstrcpy (w, c)
     register w_char *w;
     register unsigned char *c;
{
  return (wnn_Sstrcpy (w, c));
}

int
Strcmp (w1, w2)
     register w_char *w1, *w2;
{
  return (wnn_Strcmp (w1, w2));
}

int
Substr (w1, w2)
     register w_char *w1, *w2;
{
  return (wnn_Substr (w1, w2));
}

int
Strncmp (w1, w2, n)
     register w_char *w1, *w2;
     register int n;
{
  return (wnn_Strncmp (w1, w2, n));
}

w_char *
Strcpy (w1, w2)
     register w_char *w1, *w2;
{
  return (wnn_Strcpy (w1, w2));
}

w_char *
Strncpy (w1, w2, n)
     register w_char *w1, *w2;
     register int n;
{
  return (wnn_Strncpy (w1, w2, 2));
}

int
Strlen (w)
     register w_char *w;
{
  return (wnn_Strlen (w));
}

#ifdef nodef
int
wstrncmp (s1, s2, n)
     register w_char *s1, *s2;
     register int n;
{
  for (; *s1 != 0 && *s2 != 0;)
    {
      if (*s1 != *s2)
        return (w_char_cmp (*s1, *s2));
      s1++;
      s2++;
      n--;
      if (n == 0)
        return (0);
    }
  if (*s1 == 0 && *s2 == 0)
    return (0);
  if (*s1 == 0)
    return (-1);
  return (1);
}

w_char_cmp (s1, s2)
     register w_char s1, s2;
{
  if (s1 == s2)
    return (0);
  if (s1 == TOUTEN_NUM)
    {
      if (s2 == KUTEN_NUM)
        return (-1);
      return (1);
    }
  if (s1 == KUTEN_NUM)
    return (1);
  if (s2 == TOUTEN_NUM || s2 == KUTEN_NUM)
    return (-1);
  if (s2 > s1)
    return (-1);
  return (1);
}
#endif
