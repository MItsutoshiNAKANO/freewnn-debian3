/*
 *  $Id: jis_in.c,v 1.3 2001/06/14 18:16:07 ura Exp $
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

/*
 *      JIS 入力のためのプログラム
 *      programs for JIS NYUURYOKU
 */

#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define FROM_JIS        0x21
#define TO_JIS          0x7E

static int get_jis_ichiran ();

int
in_jis ()                       /*  returns code for a moji  */
{
  char buffer[5];
  int code;
  int c_p = 0;                  /* 何文字はいったかを示す */
  int c, k;
  extern int t_quit ();
  extern int henkan_off ();
  int not_redrawtmp = not_redraw;
  not_redraw = 1;               /*リドローしない */

  for (k = 0; k < 5; k++)
    {
      buffer[k] = ' ';
    }

completely_start:
  throw_c (0);                  /* モード表示の後に出す */
  printf (MSG_GET (6));
  /*
     printf("JISコード: ");
   */
  clr_line ();
  flush ();
start:
  while (1)
    {

      throw_c (10);
      h_r_on ();
      for (k = 0; k < 4; k++)
        {
          putchar (buffer[k]);
        }
      h_r_off ();
      clr_line ();
      flush ();

      c = keyin ();
      if ((c == ESC) || (t_quit == main_table[5][c]))
        {
          throw_c (0);
          clr_line ();
          flush ();
          not_redraw = not_redrawtmp;
          return (-1);
        }
      else if (henkan_off == main_table[5][c])
        {                       /*きたない! */
          henkan_off ();
        }
      else if (c_p < 4 && ((c <= '9' && c >= '0') || (c <= 'F' && c >= 'A') || (c <= 'f' && c >= 'a')))
        {
          if ((c_p == 0 || c_p == 2) ? (c <= 'F' && c >= 'A') ||        /* For EUC */
              (c <= 'f' && c >= 'a') || /* For EUC */
              (c <= '7' && c >= '2') :
              !(((buffer[c_p - 1] == '2' ||
                  buffer[c_p - 1] == 'A' || buffer[c_p - 1] == 'a') && c == '0') || ((buffer[c_p - 1] == '7' || buffer[c_p - 1] == 'F' || buffer[c_p - 1] == 'f') && (c == 'F' || c == 'f'))))
            {
              buffer[c_p++] = (char) c;
            }
          else
            {
              ring_bell ();
            }
        }
      else if (c == rubout_code && c_p)
        {
          buffer[--c_p] = ' ';
        }
      else if (c == rubout_code && c_p == 0)
        {
          throw_c (0);
          clr_line ();
          flush ();
          not_redraw = not_redrawtmp;
          return (-1);
        }
      else if (c == NEWLINE || c == CR)
        {
          if (c_p <= 1)
            {
              goto start;
            }
          else if (c_p == 4)
            {
              code = 0;
              for (k = 0; k < 4; k++)
                {
                  code = code * 16 + hextodec (buffer[k]);
                }
              code |= 0x8080;   /* ujis にして返す */
              break;
            }
          if ((code = get_jis_ichiran (buffer, c_p)) == -1)
            {
              goto completely_start;
            }
          break;
        }
      else
        {
          ring_bell ();
        }
    }
  throw_c (0);
  clr_line ();
  not_redraw = not_redrawtmp;
  return (code);
}

int
hextodec (c)
     char c;
{
  if (c >= '0' && c <= '9')
    {
      return (c - '0');
    }
  else if (c >= 'A' && c <= 'F')
    {
      return (c - 'A' + 10);
    }
  else if (c >= 'a' && c <= 'f')
    {
      return (c - 'a' + 10);
    }
  else
    return (-1);
}


static int
get_jis_ichiran (st, num)
     char *st;
     int num;
{
  int from;
  int i;
  char *c;
  int maxh;
  int code;
  char buf[512];
  char *buf_ptr[96];

  if (num == 2)
    {
      from = hextodec (st[0]) << (8 + 4) | hextodec (st[1]) << 8 | 0x21;
      from |= 0x8080;
      maxh = 94;
    }
  else
    {
      from = hextodec (st[0]) << (8 + 4) | hextodec (st[1]) << 8 | hextodec (st[2]) << 4;
      from |= 0x8080;
      maxh = 16;
      if ((from & 0xf0) == 0xf0)
        {
          maxh--;
        }
      else if ((from & 0xf0) == 0xa0)
        {
          maxh--;
          from++;
        }
    }

  for (i = 0, c = buf; i < maxh; i++)
    {
      code = from + i;
      buf_ptr[i] = c;
      *c++ = (code & 0xff00) >> 8;
      *c++ = (code & 0x00ff);
      *c++ = 0;
    }
  if ((i = select_one_element (buf_ptr, maxh, 0, "", 0, 0, main_table[4])) == -1)
    {
      return (-1);
    }
  return (from + i);
}
