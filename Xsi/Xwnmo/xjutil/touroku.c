/*
 * $Id: touroku.c,v 1.9.2.1 1999/02/08 07:43:35 yamasita Exp $
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
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"
#include "xext.h"

static int touroku_start, touroku_end;
/* these are used only for making yomi string for registered kanji */


int touroku_mode = 0;

static void
freeze_messages ()
{
  register int i;

  for (i = 0; i < JCLIENTS; i++)
    {
      freeze_box (cur_root->jutil->mes_button[i]);
    }
}

static void
unfreeze_messages ()
{
  register int i;

  for (i = 0; i < JCLIENTS; i++)
    {
      unfreeze_box (cur_root->jutil->mes_button[i]);
    }
}

static int
change_mode (mode)
     int mode;
{
  int ret = EXECUTE;

  if (mode == MESSAGE1)
    {
      ret = KANJI_IN_START;
    }
  else if (mode == MESSAGE2)
    {
      ret = YOMI_IN;
    }
  else if (mode == MESSAGE3)
    {
      ret = HINSI_IN;
    }
  else if (mode == MESSAGE4)
    {
      ret = JISHO_IN;
    }
  return (ret);
}

static void
redraw_old_message (mode)
     int mode;
{
  register JutilRec *jutil = cur_root->jutil;

  if (mode == MESSAGE0)
    {
      unfreeze_box (jutil->button[EXEC_W]);
      if (jutil->button[EXEC_W]->in)
        {
          reverse_box (jutil->button[EXEC_W], jutil->button[EXEC_W]->invertgc);
          jutil->button[EXEC_W]->in = 0;
        }
    }
  else
    {
      changecolor_box (jutil->mes_button[mode], 0, 0, cur_root->bg, BoxBorderPixel);
      jutil->mes_button[mode]->in = (char) 0;
    }
}

static void
redraw_new_message (mode)
     int mode;
{
  register JutilRec *jutil = cur_root->jutil;

  if (mode == MESSAGE0)
    {
      if (!jutil->button[EXEC_W]->in)
        {
          reverse_box (jutil->button[EXEC_W], jutil->button[EXEC_W]->invertgc);
          jutil->button[EXEC_W]->in = 1;
        }
      freeze_box (jutil->button[EXEC_W]);
    }
  else
    {
      changecolor_box (jutil->mes_button[mode], 0, 0, cur_root->fg, BoxBorderPixel);
      jutil->mes_button[mode]->in = (char) 1;
    }
}

static void
xw_exec_mode ()
{
  unsigned int c1;
  int ret;
  JutilRec *jutil = cur_root->jutil;

  jutil->mes_mode = MESSAGE0;
  redraw_new_message (jutil->mes_mode);
  freeze_box (jutil->button[EXEC_W]);
  for (;;)
    {
      c1 = keyin ();
      if (xjutil->sel_ret == -2)
        {
          touroku_mode = CANCEL;
          xjutil->sel_ret = -1;
          break;
        }
      else if (xjutil->sel_ret == 0)
        {
          touroku_mode = TOUROKU_GO;
          xjutil->sel_ret = -1;
          break;
        }
      else if (xjutil->sel_ret == 1)
        {
          touroku_mode = change_mode (jutil->mode);
          xjutil->sel_ret = -1;
          break;
        }
      else if ((c1 < 256) && (main_table[9][c1] != NULL))
        {
          if ((ret = (*main_table[9][c1]) ()) == 1)
            {
              if (jutil->mes_mode == MESSAGE0)
                {
                  touroku_mode = TOUROKU_GO;
                }
              else
                {
                  jutil->mode = jutil->mes_mode;
                  touroku_mode = change_mode (jutil->mes_mode);
                }
              break;
            }
          else if (ret == -1)
            {
              touroku_mode = CANCEL;
              break;
            }
        }
      else
        {
          ring_bell ();
        }
    }
  redraw_old_message (jutil->mes_mode);
  if (jutil->mes_mode == MESSAGE0)
    {
      unfreeze_box (jutil->button[EXEC_W]);
    }
/*
        if (cur_root->jutil->button[EXEC_W]->in) {
            reverse_box(jutil->button[EXEC_W], jutil->button[EXEC_W]->invertgc);
            jutil->button[EXEC_W]->in = 0;
        }
    }
*/
}

int
hani_settei_normal (c_b)
     ClientBuf *c_b;
{
  c_b->hanten = 0x04;
  c_b->t_c_p = 0;
  hanten_jutil_mes_title (MESSAGE1, 1);
  c_b->t_m_start = -1;

  c_b->key_table = main_table[5];
  c_b->rk_clear_tbl = romkan_clear_tbl[5];
  c_b->key_in_fun = NULL;
  c_b->redraw_fun = redraw_nisemono;
  c_b->ctrl_code_fun = NULL;
  init_screen ();
  return (0);
}

#ifdef  CHINESE
int
hani_settei_yincod (c_b)
     ClientBuf *c_b;
{
  c_b->hanten = 0x04 | 0x40;
  c_b->t_c_p = 0;
  hanten_jutil_mes_title (MESSAGE1, 1);
  c_b->t_b_st = c_b->t_c_p;
  c_b->t_b_end = c_b->t_c_p + 1;
  c_b->t_m_start = c_b->t_c_p + 1;
  kk_cursor_invisible ();

  c_b->key_table = main_table[5];
  c_b->rk_clear_tbl = romkan_clear_tbl[5];
  c_b->key_in_fun = NULL;
  c_b->redraw_fun = redraw_nisemono;
  c_b->ctrl_code_fun = NULL;
  init_screen ();
  c_b->t_m_start = -1;
  return (0);
}
#endif /* CHINESE */

static int
xw_hani_settei (buffer, buflen)
     w_char *buffer;
     int buflen;
{
  ClientBuf *c_btmp, c_b1;

  c_btmp = c_b;
  c_b = &c_b1;

  c_b->buffer = buffer;
  c_b->buflen = buflen;

  c_b->maxlen = Strlen (buffer);

  freeze_messages ();
  (*hani_settei_func) (c_b);
  if (buffer_in () == -1)
    {
      if (touroku_mode == KANJI_IN_START || touroku_mode == KANJI_IN_END)
        touroku_mode = CANCEL;
      c_b = c_btmp;
      return (-1);
    }
  unfreeze_messages ();
  if (xjutil->sel_ret == -2)
    {
      touroku_mode = CANCEL;
      xjutil->sel_ret = -1;
      c_b = c_btmp;
      return (-1);
    }
  else if (xjutil->sel_ret == 0)
    {
      touroku_mode = TOUROKU_GO;
      xjutil->sel_ret = -1;
      c_b = c_btmp;
      return (0);
    }
  else if (xjutil->sel_ret == 1)
    {
      touroku_mode = change_mode (cur_root->jutil->mode);
      if (touroku_mode == KANJI_IN_START)
        {
          c_b = c_btmp;
          return (0);
        }
    }
  else
    {
      touroku_mode = EXECUTE;
    }
  Strncpy (buffer, buffer + c_b->t_m_start, c_b->t_c_p - c_b->t_m_start);
  buffer[c_b->t_c_p - c_b->t_m_start] = 0;
  touroku_start = c_b->t_m_start;
  touroku_end = c_b->t_c_p;
  c_b = c_btmp;
  return (c_b1.t_c_p - c_b1.t_m_start);
}

void
touroku ()
{
  int i;
  int ud_no, save_ud_no = -1, ud_table[WNN_MAX_JISHO_OF_AN_ENV];
  w_char yomibuf[LENGTHYOMI + 1];
  char *message[4];
  char *message1[4];
  w_char hani_buffer[1024];
  w_char get_buffer[1024];
  char dic_name_heap[2048];
  char save_text[WNN_HINSI_NAME_LEN];
  char *hp = dic_name_heap;
  int hinsi = -1, save_hinsi = -1;
  int tmp;
  char *dict_name[JISHOKOSUU];
  static int current_dic = -1;
  int current = 0;
  int size;
  int kanji_set = 0, yomi_set = 0, hinsi_set = 0, jisho_set = 0;

  cur_bnst_ = get_touroku_data (get_buffer);
  if (update_dic_list () == -1)
    {
      errorkeyin ();
      return;
    }
  for (i = 0, size = 0; i < dic_list_size; i++)
    {
      if ((dicinfo[i].type == WNN_UD_DICT || dicinfo[i].type == WNN_REV_DICT || dicinfo[i].type == CWNN_REV_DICT) && dicinfo[i].rw == 0)
        {
          if (*dicinfo[i].comment)
            {
              sStrcpy (hp, dicinfo[i].comment);
              dict_name[size] = hp;
              hp += strlen (hp) + 1;
            }
          else
            {
              dict_name[size] = dicinfo[i].fname;
            }
          ud_table[size] = i;
          if (current_dic != -1 && dicinfo[i].dic_no == current_dic)
            current = size;
          size++;
        }
    }
  if (size == 0)
    {
      print_msg_getc (msg_get (cd, 99, default_message[99], xjutil->lang), NULL, NULL, NULL);
      return;
    }
  save_ud_no = 0;

  if ((hinsi = get_default_hinsi (save_text)) != -1)
    save_hinsi = hinsi;

  message[0] = msg_get (cd, 93, default_message[93], xjutil->lang);
  message[1] = msg_get (cd, 95, default_message[95], xjutil->lang);
  message[2] = msg_get (cd, 96, default_message[96], xjutil->lang);
  message[3] = msg_get (cd, 97, default_message[97], xjutil->lang);
  message1[0] = message1[1] = message1[2] = message1[3] = NULL;
#ifdef nodef
  message1[2] = hinsi_name;
#endif
  if (size == 1)
    {
      ud_no = 0;
      message1[3] = dict_name[ud_no];
    }

  init_jutil (msg_get (cd, 92, default_message[92], xjutil->lang), msg_get (cd, 92, default_message[92], xjutil->lang), 4, message, message1);
  touroku_mode = KANJI_IN_START;

repeat:
  for (;;)
    {
      switch (touroku_mode)
        {
        case KANJI_IN_START:
          kanji_set = 0;
          change_cur_jutil (MESSAGE1);
          change_mes_title (MESSAGE1, msg_get (cd, 93, default_message[93], xjutil->lang), 1);
          kk_cursor_invisible ();
          JWMline_clear (0);
          kk_cursor_normal ();
          hani_buffer[0] = 0;
          Strcpy (hani_buffer, get_buffer);
          if ((tmp = xw_hani_settei (hani_buffer, 1024)) < 0)
            break;
          if ((tmp = Strlen (hani_buffer)) >= 64)
            {
              print_msg_getc (msg_get (cd, 100, default_message[100], xjutil->lang), NULL, NULL, NULL);
              touroku_mode = KANJI_IN_START;
            }
          else if (tmp > 0)
            {
              kanji_set = 1;
            }
          kk_cursor_invisible ();
          break;
        case YOMI_IN:
          yomi_set = 0;
          for (i = 0; i < LENGTHYOMI + 1; i++)
            {
              yomibuf[i] = 0;
            }
          change_cur_jutil (MESSAGE2);
          hanten_jutil_mes_title (MESSAGE2, 1);
          JWMline_clear (0);
          kk_cursor_normal ();
          xjutil->sel_ret = -1;
          freeze_messages ();
          tmp = kana_in ((0x08 | 0x20), yomibuf, LENGTHYOMI);
          unfreeze_messages ();
          if (Strlen (yomibuf) > 0)
            yomi_set = 1;
          if (tmp == -1 || xjutil->sel_ret == -2)
            {
              touroku_mode = CANCEL;
            }
          else if (xjutil->sel_ret == 0)
            {
              touroku_mode = TOUROKU_GO;
            }
          else if (xjutil->sel_ret == 1)
            {
              touroku_mode = change_mode (cur_root->jutil->mode);
            }
          else
            {
              touroku_mode = EXECUTE;
            }
          hanten_jutil_mes_title (MESSAGE2, 0);
          kk_cursor_invisible ();
          break;
        case HINSI_IN:
          hinsi_set = 0;
          change_cur_jutil (MESSAGE3);
          hanten_jutil_mes_title (MESSAGE3, 1);
          JWMline_clear (0);
          kk_cursor_invisible ();
          if ((hinsi = hinsi_in (save_text)) == -1)
            {
              if (save_hinsi != -1)
                {
                  hinsi = save_hinsi;
                  JWMline_clear (0);
                  xw_jutil_write_msg (save_text);
                  hinsi_set = 1;
                }
            }
          else
            {
              save_hinsi = hinsi;
              hinsi_set = 1;
            }
          hanten_jutil_mes_title (MESSAGE3, 0);
          if (touroku_mode == HINSI_IN)
            touroku_mode = EXECUTE;
          break;
        case JISHO_IN:
          if (size == 1)
            {
              touroku_mode = EXECUTE;
              jisho_set = 1;
              break;
            }
          jisho_set = 0;
          change_cur_jutil (MESSAGE4);
          hanten_jutil_mes_title (MESSAGE4, 1);
          JWMline_clear (0);
          kk_cursor_invisible ();
          ud_no = xw_select_one_element_call (dict_name, size, current, msg_get (cd, 98, default_message[98], xjutil->lang));
          if (ud_no == -1)
            {
              if (save_ud_no != -1)
                {
                  ud_no = save_ud_no;
                  JWMline_clear (0);
                  xw_jutil_write_msg (dict_name[ud_no]);
                  jisho_set = 1;
                }
            }
          else
            {
              save_ud_no = ud_no;
              xw_jutil_write_msg (dict_name[ud_no]);
              jisho_set = 1;
            }
          hanten_jutil_mes_title (MESSAGE4, 0);
/*
                if (ud_no == -3) {
                        touroku_mode = EXECUTE;
                } else if (ud_no == -1) {
                        touroku_mode = CANCEL;
                } else {
                        xw_jutil_write_msg(dict_name[ud_no]);
                }
*/
          if (touroku_mode == JISHO_IN)
            touroku_mode = EXECUTE;
          break;
        case EXECUTE:
        case TOUROKU_GO:
          if (!kanji_set)
            {
              touroku_mode = KANJI_IN_START;
              break;
            }
          else if (!yomi_set)
            {
              touroku_mode = YOMI_IN;
              break;
            }
          else if (!hinsi_set)
            {
              touroku_mode = HINSI_IN;
              break;
            }
          else if (!jisho_set)
            {
              touroku_mode = JISHO_IN;
              break;
            }
          if (touroku_mode == EXECUTE)
            {
              xw_exec_mode ();
            }
          else
            {
              goto do_touroku;
            }
          break;
        case CANCEL:
          end_jutil ();
          return;
        }
    }

do_touroku:
  current_dic = dicinfo[ud_table[ud_no]].dic_no;

  if (js_word_add (cur_env, current_dic, yomibuf, hani_buffer, "", hinsi, 0) == -1)
    {
      if_dead_disconnect (cur_env, -1);
      errorkeyin ();
      touroku_mode = EXECUTE;
      goto repeat;
    }
  end_jutil ();
  return;
}


static int
find_end_of_tango (c)
     int c;
{
  int k;
  int jisyu;                    /* 0: katakana 1: hiragana 2:ascii 3:kanji */

  if (KATAP (c_b->buffer[c]))
    jisyu = 0;
  else if (HIRAP (c_b->buffer[c]))
    jisyu = 1;
  else if (ASCIIP (c_b->buffer[c]))
    jisyu = 2;
  else if (KANJIP (c_b->buffer[c]))
    jisyu = 3;
  else
    return (c + 1);

  for (k = c + 1; k < c_b->maxlen; k++)
    {
      if (jisyu == 0)
        {
          if (!KATAP (c_b->buffer[k]))
            break;
        }
      else if (jisyu == 1)
        {
          if (!HIRAP (c_b->buffer[k]))
            break;
        }
      else if (jisyu == 2)
        {
          if (!ASCIIP (c_b->buffer[k]))
            break;
        }
      else if (jisyu == 3)
        {
          if (!KANJIP (c_b->buffer[k]))
            break;
        }
    }
  return (k);
}

int
t_markset ()
{
  if (xjutil->sel_ret == -2)
    {
      touroku_mode = CANCEL;
      hanten_jutil_mes_title (MESSAGE1, 0);
      xjutil->sel_ret = -1;
      return (-1);
    }

  if (c_b->t_m_start == -2)
    {
      hanten_jutil_mes_title (MESSAGE1, 0);
      return (0);
    }
  if (c_b->t_m_start == -1)
    {
      if (cur_root->jutil->mode != MESSAGE1)
        {
          touroku_mode = KANJI_IN_START;
          change_mes_title (MESSAGE1, msg_get (cd, 93, default_message[93], xjutil->lang), 1);
          return (0);
        }
      if (c_b->t_c_p == c_b->maxlen)
        {
          touroku_mode = KANJI_IN_END;
          change_mes_title (MESSAGE1, msg_get (cd, 94, default_message[94], xjutil->lang), 1);
          return (0);
        }
      c_b->t_m_start = c_b->t_c_p;
      touroku_mode = KANJI_IN_END;
      change_mes_title (MESSAGE1, msg_get (cd, 94, default_message[94], xjutil->lang), 1);
      call_t_redraw_move_1 (find_end_of_tango (c_b->t_c_p), c_b->t_c_p, find_end_of_tango (c_b->t_c_p), 0, 0, 0, 2);
      kk_cursor_invisible ();
      return (0);
    }
  else
    {
      if (c_b->t_m_start == c_b->t_c_p)
        {
          c_b->t_m_start = -1;
          touroku_mode = KANJI_IN_START;
          change_mes_title (MESSAGE1, msg_get (cd, 93, default_message[93], xjutil->lang), 1);
          kk_cursor_normal ();
          return (-1);
        }
      else
        {
          if (cur_root->jutil->mode == MESSAGE1)
            cur_root->jutil->mode = MESSAGE2;
          hanten_jutil_mes_title (MESSAGE1, 0);
          return (1);
        }
    }
}

int
xw_next_message ()
{
  register JutilRec *jutil = cur_root->jutil;

  redraw_old_message (jutil->mes_mode);
  if (jutil->mes_mode == MESSAGE4)
    {
      jutil->mes_mode = MESSAGE0;
    }
  else
    {
      jutil->mes_mode += 1;
    }
  redraw_new_message (jutil->mes_mode);
  return (0);
}

int
xw_previous_message ()
{
  register JutilRec *jutil = cur_root->jutil;

  redraw_old_message (jutil->mes_mode);
  if (jutil->mes_mode == MESSAGE0)
    {
      jutil->mes_mode = MESSAGE4;
    }
  else
    {
      jutil->mes_mode -= 1;
    }
  redraw_new_message (jutil->mes_mode);
  return (0);
}
