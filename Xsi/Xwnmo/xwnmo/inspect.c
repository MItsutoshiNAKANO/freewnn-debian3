/*
 * $Id: inspect.c,v 1.8.2.1 1999/02/08 08:08:48 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright 1991, 1992 by Massachusetts Institute of Technology
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
 *
 * Last modified date: 8,Feb.1999
 *
 * Code:
 *
 */
/*      Version 4.0
 */
#include <stdio.h>
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "config.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

#define LOCAL_TBL_CNT   2

int (*local_tbl[LOCAL_TBL_CNT]) () =
{
inspectdel, inspectuse};

static int
xw_inspect (c, in)
     char *c;
     int in;
{
  static int select_step = 0;
  int d;
  int ret;
  static int (*func) ();

  if (select_step == 0)
    {
      if (init_inspect ((unsigned char *) c))
        {
          ring_bell ();
          return (-1);
        }
      select_step++;
      return (BUFFER_IN_CONT);
    }
  if (select_step == 1)
    {
      if (in == -99)
        {
          end_inspect ();
          select_step = 0;
          return (-1);
        }
      if (!xim->cur_j_c_root->inspect->map)
        return (BUFFER_IN_CONT);
      if (xim->sel_ret == -2)
        {
          xim->sel_ret = -1;
          end_inspect ();
          select_step = 0;
          return (-1);
        }
      else if (xim->sel_ret != -1)
        {
          d = xim->sel_ret;
          xim->sel_ret = -1;
          if (d >= 0 && d < LOCAL_TBL_CNT && (func = local_tbl[d]))
            {
              lock_inspect ();
              select_step++;
            }
          else
            {
              return (BUFFER_IN_CONT);
            }
        }
      else if ((in < 256) && (func = main_table[9][in]))
        {
          for (d = 0; d < LOCAL_TBL_CNT; d++)
            {
              if (local_tbl[d] == func)
                {
                  lock_inspect ();
                  break;
                }
            }
          select_step++;
        }
      else
        {
          ring_bell ();
          return (BUFFER_IN_CONT);
        }
    }
  if (select_step == 2)
    {
      ret = (*func) (in);
      if (ret == 1 || ret == -1)
        {
          end_inspect ();
          select_step = 0;
          return (0);
        }
      else if (ret == BUFFER_IN_CONT)
        {
          return (BUFFER_IN_CONT);
        }
      else
        {
          select_step--;
          unlock_inspect ();
          return (BUFFER_IN_CONT);
        }
    }
  return (BUFFER_IN_CONT);
}

int
inspect (bun_no, c, in)
     int bun_no;
     char *c;
     int in;
{
  if (xw_inspect (c, in) == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
  return (1);
}

static int next_flag;
#define I_NEXT 1
#define I_PREV 2
#define I_CUR 3

static int
update_dic_list (buf)
     struct wnn_buf *buf;
{
  if ((dic_list_size = jl_dic_list (bun_data_, &dicinfo)) == -1)
    {
      errorkeyin ();
      return (-1);
    }
  return (0);
}

static int
find_dic_by_no (dic_no)
     int dic_no;
{
  int j;

  for (j = 0; j < dic_list_size; j++)
    {
      if (dicinfo[j].dic_no == dic_no)
        return (j);
    }
  return (-1);
}

static int
dic_nickname (dic_no, buf)
     int dic_no;
     char *buf;
{
  int j;

  if ((j = find_dic_by_no (dic_no)) == -1)
    return (-1);
  if (*dicinfo[j].comment)
    sStrcpy (buf, dicinfo[j].comment);
  else
    strcpy (buf, dicinfo[j].fname);
  return (0);
}

static int Bun_no;

static int
make_string_for_ke (bun_no, buf, buf_size)
     int bun_no;
     char *buf;
     int buf_size;
{
  struct wnn_jl_bun *bun;
  w_char buf1[1024];
  w_char save_c;
  char tmp[WNN_HINSI_NAME_LEN * 2];
  char *lang;

  lang = cur_lang->lang;

  Bun_no = bun_no;
  bun = bun_data_->bun[bun_no];
  jl_get_kanji (bun_data_, bun_no, bun_no + 1, buf1);
  if (print_out_func)
    (*print_out_func) (buf1, buf1, Strlen (buf1));
  delete_w_ss2 (buf1, Strlen (buf1));
  sStrcpy (buf, buf1);
  strcat (buf, " ");
  jl_get_yomi (bun_data_, bun_no, bun_no + 1, buf1);
  if (print_out_func)
    (*print_out_func) (buf1, buf1, Strlen (buf1));
  delete_w_ss2 (buf1, Strlen (buf1));
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
          sprintf (buf + strlen (buf), " %s:%c%d", msg_get (cd, 4, default_message[4], lang), bun->ima ? '*' : ' ', bun->hindo);
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
  sprintf (buf + strlen (buf), "%s:%d %s:%d", msg_get (cd, 5, default_message[5], lang), bun->hyoka, msg_get (cd, 6, default_message[6], lang), bun->kangovect);


  if (bun->dai_top)
    {
      if (bun->daihyoka != -1)
        {
          sprintf (buf + strlen (buf), "(%s:%d) ", msg_get (cd, 7, default_message[7], lang), bun->daihyoka);
        }
      else
        {
          sprintf (buf + strlen (buf), "(%s:---) ", msg_get (cd, 7, default_message[7], lang));
        }
    }

  return (0);
}

int
inspect_kouho (in)
     int in;
{
  static char buf[1024];
  static WnnClientRec *c_c_sv = 0;

#ifdef  CALLBACKS
  if (IsPreeditCallbacks (cur_x) && cur_x->cb_redraw_needed)
    {
      t_print_l ();
      c_c_sv = 0;
      pop_func (c_c);
      cur_x->cb_redraw_needed = 0;
      return (0);
    }
#endif /* CALLBACKS */

  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (0);
    }
  if (c_c_sv == 0)
    {
      c_c_sv = c_c;
      push_func (c_c, inspect_kouho);
      next_flag = I_CUR;
      if (make_string_for_ke (cur_bnst_, buf, sizeof (buf)) == -1)
        {
          print_msg_getc ("Error in inspect", NULL, NULL, NULL);
          return (0);
        }
    }
  if (inspect (cur_bnst_, buf, in) == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
#ifdef  CALLBACKS
  if (IsPreeditCallbacks (cur_x))
    {
      cur_x->cb_redraw_needed = 1;
      SendCBRedraw ();
      return (BUFFER_IN_CONT);
    }
#endif /* CALLBACKS */
  t_print_l ();
  c_c_sv = 0;
  pop_func (c_c);
  return (0);
}

static int
sakujo_kouho1 (bun_no, in)
     int bun_no;
     int in;
{
  w_char w_buf[512];
  char buf[512];
  int kanji_len, ret;
  char *lang;
  static int step = 0;

  if (step == 0)
    {
      lang = cur_lang->lang;
      jl_get_kanji (bun_data_, bun_no, bun_no + 1, w_buf);
      kanji_len = jl_kanji_len (bun_data_, bun_no, bun_no + 1) - jl_fuzoku_len (bun_data_, bun_no);
      w_buf[kanji_len] = 0;
      sStrcpy (buf, w_buf);
      strcat (buf, " ");
      strcat (buf, msg_get (cd, 8, default_message[8], lang));
      strcat (buf, ":");
      jl_get_yomi (bun_data_, bun_no, bun_no + 1, w_buf);
      w_buf[jl_jiri_len (bun_data_, bun_no)] = 0;
      sStrcpy (buf + strlen (buf), w_buf);
      strcat (buf, " ");
      sprintf (buf + strlen (buf), "%s?", msg_get (cd, 9, default_message[9], lang));
      step++;
    }
  if ((ret = yes_or_no (buf, in)) != BUFFER_IN_CONT)
    {
      if (ret == 1)
        {
          if (jl_word_delete (bun_data_, bun_data_->bun[bun_no]->dic_no, bun_data_->bun[bun_no]->entry) == -1)
            {
              errorkeyin ();
            }
        }
      step = 0;
    }
  return (ret);
}

int
sakujo_kouho (in)
     int in;
{
  static WnnClientRec *c_c_sv = 0;

#ifdef  CALLBACKS
  if (IsPreeditCallbacks (cur_x) && cur_x->cb_redraw_needed)
    {
      t_print_l ();
      c_c_sv = 0;
      pop_func (c_c);
      cur_x->cb_redraw_needed = 0;
      return (0);
    }
#endif /* CALLBACKS */

  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (0);
    }
  if (c_c_sv == 0)
    {
      c_c_sv = c_c;
      push_func (c_c, sakujo_kouho);
    }
  if (sakujo_kouho1 (cur_bnst_, in) == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
#ifdef  CALLBACKS
  if (IsPreeditCallbacks (cur_x))
    {
      cur_x->cb_redraw_needed = 1;
      SendCBRedraw ();
      return (BUFFER_IN_CONT);
    }
#endif /* CALLBACKS */
  t_print_l ();
  c_c_sv = 0;
  pop_func (c_c);
  return (0);
}

int
inspectdel (in)
     int in;
{
  struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
  w_char buf1[1024];
  char buf[512];
  int type, ret;
  int kanji_len;
  char *lang;
  static int step = 0;

  if (step == 0)
    {
      lang = cur_lang->lang;
      type = dicinfo[find_dic_by_no (bun->dic_no)].type;
      if (type != WNN_UD_DICT && type != WNN_REV_DICT && type != CWNN_REV_DICT)
        {
          print_msg_getc ("%s", msg_get (cd, 10, default_message[10], lang), NULL, NULL);
          return (0);
        }
      if (dicinfo[find_dic_by_no (bun->dic_no)].rw == WNN_DIC_RDONLY)
        {
          print_msg_getc ("%s", msg_get (cd, 11, default_message[11], lang), NULL, NULL);
          return (0);
        }

      jl_get_kanji (bun_data_, Bun_no, Bun_no + 1, buf1);
      kanji_len = jl_kanji_len (bun_data_, Bun_no, Bun_no + 1) - jl_fuzoku_len (bun_data_, Bun_no);
      buf1[kanji_len] = 0;

      strcpy (buf, msg_get (cd, 12, default_message[12], lang));
      sStrcpy (buf + strlen (buf), buf1);
      sprintf (buf + strlen (buf), "%s", msg_get (cd, 13, default_message[13], lang));
      set_escape_code (buf);    /* ADD KURI */
      step++;
    }
  if ((ret = yes_or_no (buf, in)) != BUFFER_IN_CONT)
    {
      if (ret == 1)
        {
          if (jl_word_delete (bun_data_, bun->dic_no, bun->entry) == -1)
            {
              errorkeyin ();
            }
        }
      step = 0;
    }
  return (ret);
}

int
inspectuse (in)
     int in;
{
  struct wnn_jl_bun *bun = bun_data_->bun[Bun_no];
  w_char buf1[1024];
  char buf[512];
  int kanji_len;
  char *lang;
  int ret;
  static int step = 0;

  if (step == 0)
    {
      lang = cur_lang->lang;
      if (dicinfo[find_dic_by_no (bun->dic_no)].hindo_rw == WNN_DIC_RDONLY)
        {
          print_msg_getc ("%s", msg_get (cd, 14, default_message[14], lang), NULL, NULL);
          return (0);
        }
      strcpy (buf, msg_get (cd, 12, default_message[12], lang));
      jl_get_kanji (bun_data_, Bun_no, Bun_no + 1, buf1);
      kanji_len = jl_kanji_len (bun_data_, Bun_no, Bun_no + 1) - jl_fuzoku_len (bun_data_, Bun_no);
      buf1[kanji_len] = 0;
      sStrcpy (buf + strlen (buf), buf1);
      set_escape_code (buf);    /* ADD KURI */
      sprintf (buf + strlen (buf), "%s", msg_get (cd, 15, default_message[15], lang));
      step++;
    }
  if ((ret = yes_or_no (buf, in)) != BUFFER_IN_CONT)
    {
      if (ret == 1)
        {
          if (jl_word_use (bun_data_, bun->dic_no, bun->entry) == -1)
            {
              errorkeyin ();
            }
        }
      step = 0;
    }
  return (ret);
}

unsigned char *
next_inspect ()
{
  char buf[1024];
  char *c = buf;

  if (jl_zenkouho_bun (bun_data_) != cur_bnst_ || jl_zenkouho_daip (bun_data_) != WNN_SHO)
    {
      jl_zenkouho (bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ);
    }
  jl_next (bun_data_);
  make_kanji_buffer (cur_bnst_);
  c_b->t_m_start = bunsetsuend[cur_bnst_];
  c_b->t_b_st = dai_top (bun_data_, cur_bnst_); /* add by KUWA */
  c_b->t_b_end = dai_end (bun_data_, cur_bnst_);        /* add by KUWA */
  if (make_string_for_ke (cur_bnst_, buf, sizeof (buf)) == -1)
    {
      print_msg_getc ("Error in inspect", NULL, NULL, NULL);
      return (NULL);
    }
  return ((unsigned char *) c);
}

unsigned char *
previous_inspect ()
{
  char buf[1024];
  char *c = buf;

  if (jl_zenkouho_bun (bun_data_) != cur_bnst_ || jl_zenkouho_daip (bun_data_) != WNN_SHO)
    {
      jl_zenkouho (bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ);
    }
  jl_previous (bun_data_);
  make_kanji_buffer (cur_bnst_);
  c_b->t_m_start = bunsetsuend[cur_bnst_];
  c_b->t_b_st = dai_top (bun_data_, cur_bnst_); /* add by KUWA */
  c_b->t_b_end = dai_end (bun_data_, cur_bnst_);        /* add by KUWA */
  if (make_string_for_ke (cur_bnst_, buf, sizeof (buf)) == -1)
    {
      print_msg_getc ("Error in inspect", NULL, NULL, NULL);
      return (NULL);
    }

  return ((unsigned char *) c);
}
