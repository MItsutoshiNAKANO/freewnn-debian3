/*
 *  $Id: kuten.c,v 1.4 2013/09/02 11:01:40 itisango Exp $
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
**  句点入力 の 為の プログラム 
**   programs for KUTEN NYUURYOKU
*/

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define MAX_TEN_LENGTH  94

static int get_ichiran (int);

int
in_kuten ()                     /*  returns code for a moji  */
{
  char buffer[6];
  int code;
  int c_p = 0;                  /* 何文字入力されたかを示す。 */
  int c, k;
  int ret1, ret2;
  int mode = -1;
  /* '.'が入力されてなかったら-1。されていたら何桁めに
     入っているかを保持する。 */
  extern int t_quit ();         /* added by Nide !! */
  extern int henkan_off ();
  int not_redrawtmp = not_redraw;
  not_redraw = 1;               /* リドローしない */

  for (k = 0; k < 6; buffer[k++] = ' ');

completely_start:
  throw_c (0);                  /* モード表示の後に出す */
  printf (MSG_GET (7));
  /*
     printf("区点 : ");
   */
  clr_line ();
  flush ();
start:
  while (1)
    {

      throw_c (7);
      h_r_on ();
      for (k = 0; k < 5; k++)
        {
          putchar (buffer[k]);
        }
      h_r_off ();
      clr_line ();
      flush ();

      c = keyin ();
      if (c < 256)
        {
          if ((c == ESC) || (t_quit == main_table[5][c]))
            {
              throw_c (0);
              clr_line ();
              flush ();
              not_redraw = not_redrawtmp;
              return (-1);
            }
          else if (henkan_off == main_table[5][c])
            {                   /*きたない! */
              henkan_off ();
            }
        }
      if ((mode >= 0 ? (c_p - mode) <= 2 : c_p < 4) && c <= '9' && c >= '0')
        {
          buffer[c_p++] = (char) c;
        }
      if ((mode == -1) && (c == (int) '.') && 0 < c_p && c_p <= 2)
        {
          mode = c_p;
          buffer[c_p++] = (char) c;
        }
      else if (c == rubout_code && c_p)
        {
          if (buffer[c_p - 1] == '.')
            mode = -1;
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
      else if (((c == NEWLINE) || (c == CR)) && (c_p >= 3) && (buffer[c_p - 1] != '.'))
        {
          if ((!(sscanf (buffer, "%d.%d", &ret1, &ret2) == 2) &&
               !((c_p == 4) && sscanf (buffer, "%2d%2d", &ret1, &ret2) == 2) && !((c_p == 3) && sscanf (buffer, "%1d%2d", &ret1, &ret2) == 2)) || ret1 <= 0 || ret1 > 94 || ret2 <= 0 || ret2 > 94)
            {
              ring_bell ();
              goto start;
            }
          code = (((ret1 + 32) | 0x80) << 8) | ((ret2 + 32) | 0x80);
          break;
        }
      else if ((c == NEWLINE || c == CR) && c_p <= 2 && mode == -1)
        {
          if (sscanf (buffer, "%d", &ret1) != 1 || (ret1 <= 0 || ret1 > 94))
            {
              goto start;
            }
          if ((code = get_ichiran (ret1)) == -1)
            {
              goto completely_start;
            }
          else
            {
              break;
            }
        }
    }
  throw_c (0);
  clr_line ();
  not_redraw = not_redrawtmp;
  return (code);
}

/** 候補一覧を表示してその中から選択する。*/
static int
get_ichiran (ku)
     int ku;                    /* 区 */
{
  char buf[1024];
  char *buf_ptr[MAX_TEN_LENGTH];
  int i;
  char *c;

  for (i = 0, c = buf; i < MAX_TEN_LENGTH; i++)
    {
      buf_ptr[i] = c;
      *c++ = (ku + 32) | 0x80;
      *c++ = ((i + 1) + 32) | 0x80;
      *c++ = 0;
    }
  if ((i = select_one_element (buf_ptr, MAX_TEN_LENGTH, 0, "", 0, 0, main_table[4])) == -1)
    {
      return (-1);
    }
  return ((((ku + 32) | 0x80) << 8) | ((i + 1) + 32) | 0x80);
}
