/*
 *  $Id: inspect.c $
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
#include "wnn_config.h"
#include "buffer.h"

int
inspect (bun_no)
     int bun_no;
{
  char buf[1024];
  char *c = buf;

  if (make_string_for_ke (bun_no, buf, sizeof (buf)) == -1)
    {
      print_msg_getc ("Error in inspect");
      return (0);
    }
  select_line_element (&c, 1, 0, "", 0, 0, main_table[9]);
  return (1);
}

static int next_flag;
#define I_NEXT 1
#define I_PREV 2
#define I_CUR 3

int
inspect_kouho ()
{
  push_cursor ();
  next_flag = I_CUR;
  while (next_flag)
    {
      if (next_flag == I_NEXT || next_flag == I_PREV)
        {
          if (jl_zenkouho_bun (bun_data_) != cur_bnst_ || jl_zenkouho_daip (bun_data_) != WNN_SHO)
            {
              jl_zenkouho (bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ);
            }
          if (next_flag == I_NEXT)
            {
              jl_next (bun_data_);
            }
          else
            {
              jl_previous (bun_data_);
            }
          make_kanji_buffer (cur_bnst_);
          c_b->t_m_start = bunsetsuend[cur_bnst_];
          c_b->t_b_st = bunsetsu[dai_top (bun_data_, cur_bnst_)];
          c_b->t_b_end = bunsetsuend[dai_end (bun_data_, cur_bnst_) - 1];
        }
      next_flag = 0;
      inspect (cur_bnst_);
    }
  pop_cursor ();
  t_print_l ();
  return (0);
}

/*
 *  Very Dirty, but we permit only this inspect function to manipulate
 *  directly the contents of bun_data_.
 */

static int Bun_no;

int
make_string_for_ke (bun_no, buf, buf_size)
     int bun_no;
     char *buf;
     int buf_size;
{
  struct wnn_jl_bun *bun;
  w_char buf1[1024];
  w_char save_c;
  char tmp[WNN_HINSI_NAME_LEN * 2];

  Bun_no = bun_no;
  bun = bun_data_->bun[bun_no];
  jl_get_kanji (bun_data_, bun_no, bun_no + 1, buf1);
  if (print_out_func)
    (*print_out_func) (buf1, buf1, Strlen (buf1));
  wnn_delete_w_ss2 (buf1, Strlen (buf1));
  sStrcpy (buf, buf1);
  strcat (buf, " ");
  jl_get_yomi (bun_data_, bun_no, bun_no + 1, buf1);
  if (print_out_func)
    (*print_out_func) (buf1, buf1, Strlen (buf1));
  wnn_delete_w_ss2 (buf1, Strlen (buf1));
  save_c = *(buf1 + bun->jirilen);
  *(buf1 + bun->jirilen) = 0;
  sStrcpy (buf + strlen (buf), buf1);
  *(buf1 + bun->jirilen) = save_c;
  strcat (buf, "-");
  sStrcpy (buf + strlen (buf), buf1 + bun->jirilen);

  set_escape_code (buf);

  strcat (buf, "   (");
  if (bun->dic_no >= 0)
    {
      update_dic_list (bun_data_);
      dic_nickname (bun->dic_no, buf + strlen (buf));
      sStrcpy (tmp, jl_hinsi_name (bun_data_, bun->hinsi));
      sprintf (buf + strlen (buf), ":%d %s", bun->entry, tmp);
      if (bun->hindo != 0x7f)
        {
          sprintf (buf + strlen (buf), MSG_GET (16), bun->ima ? '*' : ' ', bun->hindo);
        }
      else
        {
          sprintf (buf + strlen (buf), " ---------");
        }                       /* Means This entry is not used.
                                   This must not appear in ordinary use! */

    }
  else
    {
      sStrcpy (tmp, jl_hinsi_name (bun_data_, bun->hinsi));
      sprintf (buf + strlen (buf), "GIJI BUNSETU:%s", tmp);
    }
  strcat (buf, ")   ");
  sprintf (buf + strlen (buf), MSG_GET (17), bun->hyoka, bun->kangovect);

  if (bun->dai_top)
    {
      if (bun->daihyoka != -1)
        {
          sprintf (buf + strlen (buf), "(%s:%d) ", MSG_GET (18), bun->daihyoka);
        }
      else
        {
          sprintf (buf + strlen (buf), "(%s:---) ", MSG_GET (18));
        }
    }

  return (0);
}

static void sakujo_kouho1 ();

int
sakujo_kouho ()
{
  push_cursor ();
  sakujo_kouho1 (cur_bnst_);
  pop_cursor ();
  t_print_l ();
  return (0);
}

static void
sakujo_kouho1 (bun_no)
     int bun_no;
{
  w_char w_buf[512];
  char buf[512];
  int kanji_len = jl_kanji_len (bun_data_, bun_no, bun_no + 1) - jl_fuzoku_len (bun_data_, bun_no);

  jl_get_kanji (bun_data_, bun_no, bun_no + 1, w_buf);
  w_buf[kanji_len] = 0;
  sStrcpy (buf, w_buf);
  strcat (buf, MSG_GET (19));
  /*
     strcat(buf,"  読み:");
   */
  jl_get_yomi (bun_data_, bun_no, bun_no + 1, w_buf);
  w_buf[jl_jiri_len (bun_data_, bun_no)] = 0;
  sStrcpy (buf + strlen (buf), w_buf);
  set_escape_code (buf);
  strcat (buf, " ");
  sprintf (buf + strlen (buf), MSG_GET (20));
  /*
     sprintf(buf + strlen(buf), "を削除しますか?(Y/N)");
   */
  if (yes_or_no (buf) == 1)
    {
      if (jl_word_delete (bun_data_, bun_data_->bun[bun_no]->dic_no, bun_data_->bun[bun_no]->entry) == -1)
        {
          errorkeyin ();
          return;
        }
    }
}

int
inspectdel ()
{
  struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
  int type;
  w_char buf1[1024];
  char buf[512];
  int kanji_len = jl_kanji_len (bun_data_, Bun_no, Bun_no + 1) - jl_fuzoku_len (bun_data_, Bun_no);

  type = dicinfo[find_dic_by_no (bun->dic_no)].type;
  if (type != WNN_UD_DICT && type != WNN_REV_DICT && type != BWNN_REV_DICT && type != CWNN_REV_DICT)
    {
      print_msg_getc (MSG_GET (21));
      /*
         print_msg_getc("固定形式の辞書のエントリは削除出来ません。(如何?)");
       */
      return (0);
    }
  if (dicinfo[find_dic_by_no (bun->dic_no)].rw == WNN_DIC_RDONLY)
    {
      print_msg_getc (MSG_GET (22));
      /*
         print_msg_getc("リードオンリーの辞書のエントリは削除出来ません。(如何?)");
       */
      return (0);
    }

  jl_get_kanji (bun_data_, Bun_no, Bun_no + 1, buf1);
  buf1[kanji_len] = 0;

  strcpy (buf, MSG_GET (23));
  /*
     strcpy(buf, "「");
   */
  sStrcpy (buf + strlen (buf), buf1);
  sprintf (buf + strlen (buf), MSG_GET (24));
  /*
     sprintf(buf + strlen(buf), "」を削除します。(Y/N)");
   */
  set_escape_code (buf);
  if (yes_or_no (buf) == 1)
    {
      if (jl_word_delete (bun_data_, bun->dic_no, bun->entry) == -1)
        {
          errorkeyin ();
          return (0);
        }
      return (1);
    }
  return (0);
}

int
inspectuse ()
{
  struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
  w_char buf1[1024];
  char buf[512];
  int kanji_len = jl_kanji_len (bun_data_, Bun_no, Bun_no + 1) - jl_fuzoku_len (bun_data_, Bun_no);

  if (dicinfo[find_dic_by_no (bun->dic_no)].hindo_rw == WNN_DIC_RDONLY)
    {
      print_msg_getc (MSG_GET (25));
      /*
         print_msg_getc("リードオンリーの頻度は変更出来ません。(如何?)");
       */
      return (0);
    }
  strcpy (buf, MSG_GET (23));
  /*
     strcpy(buf, "「");
   */
  jl_get_kanji (bun_data_, Bun_no, Bun_no + 1, buf1);
  buf1[kanji_len] = 0;
  sStrcpy (buf + strlen (buf), buf1);
  set_escape_code (buf);
  sprintf (buf + strlen (buf), MSG_GET (26));
  /*
     sprintf(buf + strlen(buf), "」の使用を中止します。(Y/N)");
   */
  if (yes_or_no (buf) == 1)
    {
      if (jl_word_use (bun_data_, bun->dic_no, bun->entry) == -1)
        {
          errorkeyin ();
          return (0);
        }
      return (1);
    }
  return (0);
}

int
next_inspect ()
{
  next_flag = I_NEXT;
  return (1);
}

int
previous_inspect ()
{
  next_flag = I_PREV;
  return (1);
}
