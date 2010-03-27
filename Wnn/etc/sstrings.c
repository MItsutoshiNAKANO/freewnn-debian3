/*
 *  $Id: sstrings.c,v 1.4 2002/07/14 04:26:57 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "commonhd.h"
#include <stdio.h>
#if STDC_HEADERS
#  include <string.h>
#else
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif /* STDC_HEADERS */
#include "wnn_os.h"
#include "wnn_string.h"

extern int eeuc_to_ieuc ();
extern int ieuc_to_eeuc ();

int
wnn_sStrcpy (c, w)
     register char *c;
     register w_char *w;
{
  register int ret;

  ret = ieuc_to_eeuc (c, w, -1);
  c[ret] = '\0';
  return (ret);
}


int
wnn_Sstrcpy (w, c)
     w_char *w;
     unsigned char *c;
{
  register int ret;

  ret = eeuc_to_ieuc (w, c, -1) / sizeof (w_char);
  w[ret] = (w_char) 0;
  return (ret);
}

#ifdef nodef
char *
wnn_Stos (c)
     w_char *c;
{
  char *c1 = (char *) c;
  for (; *c; c++)
    {
      if (ASCIIP (*c))
        {
          *c1++ = *c;
        }
      else
        {
          *c1++ = (*c << 8);
          *c1++ = *c;
        }
    }
  return ((char *) c);
}
#endif

char *
wnn_sStrncpy (s1, s2, n)
     register char *s1;
     register w_char *s2;
     register int n;
{
  eeuc_to_ieuc (s1, s2, n / sizeof (w_char));
  return s1;
}

#ifdef CHINESE
int
wnn_Sstrcat (w, c)
     w_char *w;
     unsigned char *c;
{
  w_char *w0 = w;
  register int ret;

  if (!c || !*c)
    return (0);
  for (; *w; w++);
  ret = eeuc_to_ieuc (w, c, strlen (c)) / sizeof (w_char);
  w[ret] = (w_char) 0;
  ret += (w - w0);
  return (ret);
}
#endif
