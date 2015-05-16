/*
 *  $Id: w_string.c,v 1.3 2001/06/14 18:16:08 ura Exp $
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
#include "sdefine.h"
#include "sheader.h"
#define CONTLINE '$'


void
w_printf (x, len)
     w_char *x;
     int len;
{
  for (; *x != 0 && len > 0; x++, len--)
    {
      w_putchar (*x);
    }
  flushw_buf ();
  if (len <= 0)
    putchar (CONTLINE);
}

int
wchartochar (w, c)
     w_char *w;
     UCHAR *c;
{
  int flg = 0;
  w_char *w0 = w;

  for (; *w != 0; w++)
    {
      if ((*w) > 0xff)
        flg = 1;
    }
  sStrcpy (c, w0);
  return (flg);
}

int
eu_columlen (c)
     unsigned char *c;
{
  int len = 0;
  w_char tmp;

  for (; *c;)
    {
      if ((*c & 0x80))
        {
          tmp = ((w_char) * c++) << 8;
          tmp |= *c++;
        }
      else
        {
          tmp = *c++;
        }
      if (hankaku (tmp))
        {
          len += 1;
        }
      else
        {
          len += 2;
        }
    }
  return (len);
}

int
sStrcpy (c, w)
     register UCHAR *c;
     register w_char *w;
{
  return (wnn_sStrcpy (c, w));
}

int
Sstrcpy (w, c)
     register w_char *w;
     register UCHAR *c;
{
  return (wnn_Sstrcpy (w, c));
}

char *
sStrncpy (c, w, n)
     register UCHAR *c;
     register w_char *w;
     register int n;
{
  return (wnn_sStrncpy (c, w, n));
}

w_char *
Strcat (w1, w2)
     register w_char *w1, *w2;
{
  return (wnn_Strcat (w1, w2));
}

w_char *
Strncat (w1, w2, n)
     register w_char *w1, *w2;
     register int n;
{
  return (wnn_Strncat (w1, w2, n));
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
  return (wnn_Strncpy (w1, w2, n));
}

int
Strlen (w)
     register w_char *w;
{
  return (wnn_Strlen (w));
}
