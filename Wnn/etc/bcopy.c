/*
 *  $Id: bcopy.c,v 1.5 2005/04/10 15:26:37 aonoto Exp $
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
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "commonhd.h"

#ifndef HAVE_BCOPY
void
bcopy (b1, b2, length)
     unsigned char *b1, *b2;
     int length;
{
  if (length <= 0)
    return;
  if (b1 < b2 && b1 + length > b2)
    {
      b2 += length;
      b1 += length;
      while (length--)
        {
          *--b2 = *--b1;
        }
    }
  else
    {
      memcpy (b2, b1, length);
    }
}
#endif /* !HAVE_BCOPY */

#ifndef HAVE_BZERO
void
bzero (b, length)
     unsigned char *b;
     int length;
{
  memset (b, 0, length);
}
#endif /* !HAVE_BZERO */

#ifndef HAVE_BCMP
int
bcmp (b1, b2, length)
     unsigned char *b1;
     unsigned char *b2;
     int length;
{
  if (length == 0)
    return 0;
  return memcmp (b1, b2, length);
}
#endif /* !HAVE_BCMP */
