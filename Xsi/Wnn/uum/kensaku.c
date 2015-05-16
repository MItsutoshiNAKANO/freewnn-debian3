/*
 *  $Id: kensaku.c $
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
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

/*
static char msg1[] = " はありません。(如何?)";
*/

/* Dirty,  but share it with select_elemet.c */
struct wnn_jdata *word_searched;

struct wnn_jdata *
word_list1 (flag, tbl)
     int flag, tbl;
{
  int k;
  char *c;
  char tmp[WNN_HINSI_NAME_LEN * 2];
  /*
     static char message[] = "読み: ";
   */
  w_char kana_buf[2048];
  char buf[16384];
  char *info[MAXJIKOUHO];
  char imatukattayo;
  int cnt;
  int rcnt;
  int tcnt;
  int type;

  throw_c (0);
  clr_line ();
  kana_buf[0] = 0;
  if (kana_in (MSG_GET (19), UNDER_LINE_MODE, kana_buf, 1024) == -1)
    {
      return (NULL);
    }

  if ((cnt = jl_word_search_by_env (bun_data_, kana_buf, &word_searched)) < 0)
    {
      errorkeyin ();
      return (NULL);
    }

  update_dic_list (bun_data_);

  /* 語数を、事前に調べる */
  if (flag)
    {
      /* flag = 1 は, UD & SD, 0 は, UD only */
      tcnt = cnt;
    }
  else
    {
      for (k = 0, tcnt = 0; k < cnt; k++)
        {
          type = dicinfo[find_dic_by_no (word_searched[k].dic_no)].type;
          if (type == WNN_UD_DICT || type == WNN_REV_DICT || type == BWNN_REV_DICT || type == CWNN_REV_DICT)
            {
              tcnt++;
            }
        }
      if (tcnt == 0)
        cnt = 0;
    }
  if (cnt == 0)
    {
      throw_c (0);
      clr_line ();
      u_s_on ();
      w_printf (kana_buf, maxlength - strlen (MSG_GET (27)) - 2);
      u_s_off ();
      printf (MSG_GET (27));
      flush ();
      keyin ();
      return (NULL);
    }
  for (c = buf, k = 0, rcnt = 0; k < cnt; k++)
    {
      type = dicinfo[find_dic_by_no (word_searched[k].dic_no)].type;
      if (flag || type == WNN_UD_DICT || type == WNN_REV_DICT || type == BWNN_REV_DICT || type == CWNN_REV_DICT)
        {
          info[rcnt] = c;
          sStrcpy (c, word_searched[k].kanji);
          if (word_searched[k].com && word_searched[k].com[0])
            {
              strcat (c, " {");
              sStrcpy (c + strlen (c), word_searched[k].com);
              strcat (c, "}");
            }
          strcat (c, " (");
          dic_nickname (word_searched[k].dic_no, c + strlen (c));
          sprintf (c + strlen (c), ":%d", word_searched[k].serial);
          strcat (c, ") ");
          if (word_searched[k].ima)
            imatukattayo = '*';
          else
            imatukattayo = ' ';
          if (word_searched[k].hindo == -1)
            {
              sprintf (c + strlen (c), "---  ");
            }
          else
            {
              sprintf (c + strlen (c), "%c%-3d  ", imatukattayo, word_searched[k].hindo);
            }
          if (!(word_searched[k].int_ima && word_searched[k].int_hindo == -1))
            {
              /* Internal Hindo exist */
              if (word_searched[k].int_hindo == -1)
                {
                  sprintf (c + strlen (c), "---");
                }
              else
                {
                  sprintf (c + strlen (c), "(%c%d) ", (word_searched[k].int_ima) ? '*' : ' ', word_searched[k].int_hindo);
                }
            }
          sStrcpy (tmp, jl_hinsi_name (bun_data_, word_searched[k].hinshi));
          strcat (c, tmp);
          rcnt++;
          sprintf (c + strlen (c), "%3d/%-3d", rcnt, tcnt);
          c += strlen (c) + 1;
        }
    }
  if ((k = select_line_element (info, rcnt, 0, "", 0, 0, main_table[tbl])) == -1)
    {
      return (NULL);
    }

  if (flag == 0)
    {
      for (rcnt = 0; k >= 0 && rcnt < cnt; rcnt++)
        {
          type = dicinfo[find_dic_by_no (word_searched[rcnt].dic_no)].type;
          if (type == WNN_UD_DICT || type == WNN_REV_DICT || type == BWNN_REV_DICT || type == CWNN_REV_DICT)
            {
              k--;
            }
        }
      k = rcnt - 1;
    }
  return (word_searched + k);
}


void
word_list7 (flag)
     int flag;
{
  word_list1 (flag, 7);
}

void
kensaku ()
{
  word_list7 (1);               /* 1 for all dicts */
}
