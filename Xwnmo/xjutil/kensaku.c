/*
 * $Id: kensaku.c,v 1.2 2001/06/14 18:16:13 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Author: OMRON SOFTWARE Co., Ltd. <freewnn@rd.kyoto.omronsoft.co.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Emacs; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Commentary:
 *
 * Change log:
 *      '99/04/19       TAOKA Satoshi - 田岡 智志<taoka@infonets.hiroshima-u.ac.jp>
 *              qsort() の宣言をコメントアウト。
 *
 * Last modified date: 19,Apr.1999
 *
 * Code:
 *
 */
/*      Version 4.0
 */
#include <stdio.h>
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

extern int (*jutil_table[2][3]) ();

/* Dirty,  but share it with select_elemet.c */
struct wnn_jdata *word_searched;
int del_point, ima_point, hindo_point, com_point, max_c_len;

static int
sort_func_ws (a, b)
     register char *a, *b;
{
  int ah, bh, ai, bi, iah, ibh, iai, ibi;
  ah = ((struct wnn_jdata *) a)->hindo;
  bh = ((struct wnn_jdata *) b)->hindo;
  iah = ((struct wnn_jdata *) a)->int_hindo;
  ibh = ((struct wnn_jdata *) b)->int_hindo;
  ai = ((struct wnn_jdata *) a)->ima;
  bi = ((struct wnn_jdata *) b)->ima;
  iai = ((struct wnn_jdata *) a)->int_ima;
  ibi = ((struct wnn_jdata *) b)->int_ima;

  if (ai == WNN_IMA_OFF && ah == WNN_ENTRY_NO_USE)
    return (1);
  if (bi == WNN_IMA_OFF && bh == WNN_ENTRY_NO_USE)
    return (-1);
  if (iai == WNN_IMA_OFF && iah == WNN_ENTRY_NO_USE)
    return (1);
  if (ibi == WNN_IMA_OFF && ibh == WNN_ENTRY_NO_USE)
    return (-1);

  if (ai != bi)
    {
      if (ai < bi)
        return (1);
      return (-1);
    }
  if (iah >= 0)
    {
      ah += iah;
      bh += ibh;
    }
  if (ah > bh)
    return (-1);
  if (ah < bh)
    return (1);
  return (0);
}

void
fill_space (p, size)
     register char *p;
     register int size;
{
  for (; size > 0; size--, p++)
    {
      if (!*p)
        *p = ' ';
    }
}

static struct wnn_jdata *
word_list1 (flag, tbl)
     int flag, tbl;
{
  int k;
  char *c;
  char tmp[WNN_HINSI_NAME_LEN * 2];
  w_char kana_buf[2048];
  char buf[16384];
  char *info[MAXJIKOUHO];
  char imatukattayo;
  int cnt;
  int rcnt;
  int tcnt;
  int type;
  int max_k_len = 0, max_d_len = 0;
  static char message4[2048];
  int ret;
  char *table_t[6];
/*    extern int qsort();*/

  if (get_env () == -1)
    return (NULL);

  table_t[0] = msg_get (cd, 70, default_message[70], xjutil->lang);
  init_jutil (msg_get (cd, 69, default_message[69], xjutil->lang), msg_get (cd, 8, default_message[8], xjutil->lang), 1, table_t, NULL);
  change_cur_jutil (MESSAGE1);
retry:
  throw_col (0);
  JWMline_clear (0);
  kk_cursor_normal ();
  kana_buf[0] = 0;
  if ((ret = kana_in (UNDER_LINE_MODE, kana_buf, 1024)) == -1)
    {
      kk_cursor_invisible ();
      end_jutil ();
      return (NULL);
    }
  kk_cursor_invisible ();
  if (kana_buf[0] == 0)
    {
      ring_bell ();
      goto retry;
    }
  if ((cnt = js_word_search_by_env (cur_env, kana_buf, &wordrb)) < 0)
    {
      if_dead_disconnect (cur_env, -1);
      end_jutil ();
      errorkeyin ();
      return (NULL);
    }
  word_searched = (struct wnn_jdata *) wordrb.buf;
  qsort ((char *) word_searched, cnt, sizeof (struct wnn_jdata), sort_func_ws);

  update_dic_list ();

  if (cnt == 0)
    {
      print_msg_getc (msg_get (cd, 71, default_message[71], xjutil->lang), NULL, NULL, NULL);
      goto retry;
    }
  /* KURI 語数を、事前に調べる */
  if (flag)
    {
      tcnt = cnt;
    }
  else
    {
      for (k = 0, tcnt = 0; k < cnt; k++)
        {
          type = dicinfo[find_dic_by_no (word_searched[k].dic_no)].type;
          if (type == WNN_UD_DICT || type == WNN_REV_DICT || type == CWNN_REV_DICT)
            {
              tcnt++;
            }
        }
    }
  max_k_len = 0;
  max_d_len = 0;
  max_c_len = 0;
  for (c = buf, k = 0; k < cnt; k++)
    {
      sStrcpy (c, word_searched[k].kanji);
      ret = strlen (c);
      if (max_k_len < ret)
        max_k_len = ret;
      if (word_searched[k].com && word_searched[k].com[0])
        {
          strcpy (c, "{");
          sStrcpy (c + strlen (c), word_searched[k].com);
          strcat (c, "}");
          ret = strlen (c);
          if (ret > max_c_len)
            max_c_len = ret;
        }
      dic_nickname (word_searched[k].dic_no, c);
      sprintf (c + strlen (c), ":%d", word_searched[k].serial);
      ret = strlen (c) + 2;
      if (max_d_len < ret)
        max_d_len = ret;
    }
  max_k_len += 2;
  max_c_len += 2;
  max_d_len += 2;
  bzero (buf, sizeof (buf));
  del_point = 0 + 1;
  com_point = del_point + 2 + max_k_len;
  ima_point = com_point + max_c_len + max_d_len;
  hindo_point = ima_point + 1;
  for (c = buf, k = 0, rcnt = 0; k < cnt; k++)
    {
      type = dicinfo[find_dic_by_no (word_searched[k].dic_no)].type;
      if (!flag && !(type == WNN_UD_DICT || type == WNN_REV_DICT || type == CWNN_REV_DICT))
        continue;
      info[rcnt] = c;
      strcpy (c, "  ");
      c += strlen (c);
      sStrcpy (c, word_searched[k].kanji);
      fill_space (c, max_k_len);
      c += max_k_len;
      if (word_searched[k].com && word_searched[k].com[0])
        {
          strcat (c, "{");
          sStrcpy (c + strlen (c), word_searched[k].com);
          strcat (c, "}");
        }
      fill_space (c, max_c_len);
      c += max_c_len;
      strcat (c, "(");
      dic_nickname (word_searched[k].dic_no, c + strlen (c));
      sprintf (c + strlen (c), ":%d", word_searched[k].serial);
      strcat (c, ")");
      fill_space (c, max_d_len);
      c += max_d_len;
      if (word_searched[k].ima)
        imatukattayo = '*';
      else
        imatukattayo = ' ';
      if (word_searched[k].hindo == -1)
        {
          sprintf (c, "----  ");
        }
      else
        {
          sprintf (c, "%c%-3d  ", (word_searched[k].ima ? '*' : ' '), word_searched[k].hindo);
        }
      c += 6;
      if (!(word_searched[k].int_ima && word_searched[k].int_hindo == -1))
        {
          /* Internal Hindo exist */
          if (word_searched[k].int_hindo == -1)
            {
              sprintf (c, "(----)  ");
            }
          else
            {
              sprintf (c, "(%c%-3d)  ", (word_searched[k].int_ima) ? '*' : ' ', word_searched[k].int_hindo);
            }
        }
      else
        {
          sprintf (c, "        ");
        }
      c += 8;
      if (js_hinsi_name (cur_env->js_id, word_searched[k].hinshi, &rb) == -1)
        {
          if_dead_disconnect (cur_env, NULL);
          return (NULL);
        }
      sStrcpy (tmp, (w_char *) (rb.buf));
      strcat (c, tmp);
      fill_space (c, 20);
      rcnt++;
      c += 20;
      sprintf (c, "  %3d/%-3d", rcnt, tcnt);
      c += strlen (c) + 1;
    }
  end_jutil ();
  sprintf (message4, "%s %s : %s", msg_get (cd, 69, default_message[69], xjutil->lang), msg_get (cd, 70, default_message[70], xjutil->lang), kana_buf);

  table_t[0] = msg_get (cd, 67, default_message[67], xjutil->lang);
  table_t[1] = msg_get (cd, 68, default_message[68], xjutil->lang);
  table_t[2] = msg_get (cd, 39, default_message[39], xjutil->lang);
  table_t[3] = msg_get (cd, 120, default_message[120], xjutil->lang);
  table_t[4] = msg_get (cd, 121, default_message[121], xjutil->lang);
  table_t[5] = msg_get (cd, 122, default_message[122], xjutil->lang);
  k = xw_select_one_element_keytable (info, rcnt, 0, msg_get (cd, 69, default_message[69], xjutil->lang), 6, table_t, jutil_table[0], main_table[tbl], NULL);
  if (k == -1 || k == -3)
    {
      return (NULL);
    }
  if (flag == 0)
    {
      for (rcnt = 0; k >= 0 && rcnt < cnt; rcnt++)
        {
          type = dicinfo[find_dic_by_no (word_searched[rcnt].dic_no)].type;
          if (type == WNN_UD_DICT || type == WNN_REV_DICT || type == CWNN_REV_DICT)
            {
              k--;
            }
        }
      k = rcnt - 1;
    }
  return (word_searched + k);
}

static void
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
