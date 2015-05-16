/*
 *  $Id: bcopy.c $
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

#include "commonhd.h"

#if defined(SYSVR2) && !defined(linux) && !defined(DGUX) && !defined(uniosu)

void
bcopy (b1, b2, length)
     register unsigned char *b1, *b2;
     register length;
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

void
bzero (b, length)
     register unsigned char *b;
     register length;
{
  memset (b, 0, length);
}

int
bcmp (b1, b2, length)
     register unsigned char *b1;
     register unsigned char *b2;
     register length;
{
  if (length == 0)
    return 0;
  return memcmp (b1, b2, length);
}
#endif /* SYSVR2 */
