/*
 *  $Id: snd_rcv.c,v 1.4 2003/05/11 18:43:16 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2003
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
#include "de_header.h"

static int fmode;
 /* JS_FILE_SEND (server receives) */
int
fopen_read_cur (fn)
     char *fn;
{
  fmode = 'r';
  return 1;
}

/* JS_FILE_RECEIVE (server sends) */
int
fopen_write_cur (fn)
     char *fn;
{
  fmode = 'w';
  return 1;
}

/* JS_FILE_SEND (server recieves) */
int
fread_cur (p, size, nitems)
     char *p;
     register int size, nitems;
{
  register int i, j, xx;
  for (i = 0; i < nitems; i++)
    {
      for (j = 0; j < size; j++)
        {
          *p++ = xx = xgetc_cur ();
          if (xx == -1)
            return i;
        }
    }
  return nitems;
}

static int store = -1;

int
xgetc_cur ()
{
  register int x;
  if (store != -1)
    {
      x = store;
      store = -1;
      return (x);
    }
  if ((x = getc_cur ()) != 0xFF)
    return x;
  if (getc_cur () == 0xFF)
    return -1;                  /* EOF */
  return 0xFF;
}

void
xungetc_cur (x)                 /* H.T. */
     int x;
{
  store = x;
}

/* JS_FILE_SRECEIVE (server sends) */
void
fwrite_cur (p, size, nitems)
     unsigned char *p;
     int size, nitems;
{
  register int i, x;
  x = size * nitems;
  for (i = 0; i < x; i++)
    {
      xputc_cur (*p++);
    }
}

void
xputc_cur (x)
     unsigned char x;           /* H.T. */
{
  putc_cur (x);
  if (x == 0xFF)
    {
      putc_cur (0x00);
    }
}

void
fclose_cur ()
{
  if (fmode != 'w')
    return;
  putc_cur (0xFF);
  putc_cur (0xFF);              /* EOF */
}
