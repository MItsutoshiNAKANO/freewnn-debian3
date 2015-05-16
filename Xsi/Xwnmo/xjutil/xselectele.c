/*
 * $Id: xselectele.c,v 1.1.2.1 1999/02/08 07:43:39 yamasita Exp $
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
#define DD_MAX 512

#include <stdio.h>
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"
#include "xext.h"

#define printh(mm)\
  if(state == 0){\
    printf("%c.",((mm - dd[cc]) > 9)? mm - dd[cc] - 10  + 'A':mm - dd[cc] + '0');\
    printf(data[mm]);\
  }else{\
    printf("%c.",((mm > 9)? mm - 10  + 'A':mm + '0'));\
    printf(data[mm]);}


#define kouho_len(x) (strlen(x) + 4)

static int cc;                  /* ima no gamen */
static int kosuu;               /* kosuu of elements */
extern hilited_item;
extern max_line;

int
xw_select_one_element_call (c, kosuu1, init, title)
     char **c;
     int kosuu1;
     int init;
     char *title;
{
  int in, ret;

  for (;;)
    {
      if ((ret = xw_select_one_element (c, kosuu1, init, title, SENTAKU, main_table[4], in)) != BUFFER_IN_CONT)
        return (ret);
      in = keyin ();
    }
}

int
xw_select_one_element_keytable (c, kosuu1, init, title, bcnt, btable_t, b_tbl, key_tbl, comment)
     char **c;
     int kosuu1;
     int init;
     char *title;
     int bcnt;
     char *btable_t[];
     int (**b_tbl) ();
     int (**key_tbl) ();
     char *comment;
{
  register int i;
  int d;
  unsigned int c1;
  int ret;

  kosuu = kosuu1;
  init_ichiran (c, kosuu1, init, title, (unsigned char *) "", (unsigned char *) "", comment, 0, SENTAKU);
  for (;;)
    {
    TOP:
      xjutil->sel_ret = -1;
      c1 = keyin ();
      if (xjutil->sel_ret == -2)
        {
          end_ichiran ();
          romkan_clear ();
          return (-1);
        }
      else if (xjutil->sel_ret != -1)
        {
          d = find_hilite ();
          xjutil->sel_ret = -1;
          if (d < 0 || d > kosuu)
            {
              ring_bell ();
              continue;
            }
        }
      else
        {
          if ((c1 < 256) && (key_tbl[c1] != NULL))
            {
              cc = d = find_hilite ();
              if (d < 0 || d > kosuu)
                {
                  ring_bell ();
                  continue;
                }
              for (i = 0; i < bcnt; i++)
                {
                  if (key_tbl[c1] == b_tbl[i])
                    {
                      lock_ichiran ();
                      break;
                    }
                }
              ret = (*key_tbl[c1]) ();
              unlock_ichiran ();
              if (ret == -1)
                {
                  end_ichiran ();
                  romkan_clear ();
                  return (-1);
                }
              else if (ret == 1)
                {
                  end_ichiran ();
                  romkan_clear ();
                  return (0);
                }
              else
                {
                  continue;
                }
            }
          else
            {
              ring_bell ();
              continue;
            }
        }
      romkan_clear ();
      if (!bcnt)
        {
          end_ichiran ();
          return (d);
        }
      else
        {
          cc = d;
          lock_ichiran ();
        }
      init_keytable (bcnt, btable_t);
      for (;;)
        {
          c1 = keyin ();
          if (xjutil->sel_ret == -2)
            {
              end_ichiran ();
              end_keytable ();
              romkan_clear ();
              return (-1);
            }
          else if (xjutil->sel_ret != -1)
            {
              d = xjutil->sel_ret;
              xjutil->sel_ret = -1;
              if ((d < bcnt) && (b_tbl[d] != NULL))
                {
                  end_keytable ();
                  if ((ret = (*b_tbl[d]) ()) == 1)
                    {
                      end_ichiran ();
                      romkan_clear ();
                      return (0);
                    }
                  unlock_ichiran ();
                  goto TOP;
                }
              else
                {
                  ring_bell ();
                }
            }
          else
            {
              if ((c1 < 256) && (key_tbl[c1] != NULL))
                {
                  end_keytable ();
                  if ((ret = (*key_tbl[c1]) ()) == 1)
                    {
                      end_ichiran ();
                      romkan_clear ();
                      return (0);
                    }
                  unlock_ichiran ();
                  goto TOP;
                }
              else
                {
                  ring_bell ();
                  xjutil->sel_ret = -1;
                  continue;
                }
            }
        }
      romkan_clear ();
    }
}

/*
 *
 *  Functions Called during select element (dictionaries).
 *
 */

/* jishodel for kensaku */
int
kdicdel ()
{
  char buf[512];

  sprintf (buf, "%s %s %s", msg_get (cd, 77, default_message[77], xjutil->lang), dicinfo[cc].fname, msg_get (cd, 78, default_message[78], xjutil->lang));
  if (yes_or_no (buf) == 1)
    {
      if (dic_delete_e (cur_env, dicinfo[cc].dic_no) == -1)
        {
          errorkeyin ();
          return (0);
        }
      return (1);
    }
  return (0);
}


int
kdicuse ()
{
  char buf[512];

  if (dicinfo[cc].enablef)
    {
      sprintf (buf, "%s %s %s", msg_get (cd, 77, default_message[77], xjutil->lang), dicinfo[cc].fname, msg_get (cd, 79, default_message[89], xjutil->lang));
    }
  else
    {
      sprintf (buf, "%s %s %s", msg_get (cd, 77, default_message[77], xjutil->lang), dicinfo[cc].fname, msg_get (cd, 80, default_message[80], xjutil->lang));
    }
  if (yes_or_no (buf) == 1)
    {
      if (js_dic_use (cur_env, dicinfo[cc].dic_no, !dicinfo[cc].enablef) == -1)
        {
          if_dead_disconnect (cur_env, -1);
          errorkeyin ();
          return (0);
        }
      return (1);
    }
  return (0);
}

int
kdiccom ()
{
  w_char com[512];
  WNN_DIC_INFO dic;
  WNN_FILE_INFO_STRUCT file;
  char *mes[1];

  com[0] = 0;
  mes[0] = msg_get (cd, 81, default_message[81], xjutil->lang);
  init_jutil (msg_get (cd, 39, default_message[39], xjutil->lang), msg_get (cd, 8, default_message[8], xjutil->lang), 1, &mes, NULL);
  cur_text->max_pos = 127;
  clr_line_all ();
  throw_col (0);
  kk_cursor_normal ();
  if (kana_in (UNDER_LINE_MODE, com, 512) == -1)
    {
      kk_cursor_invisible ();
      end_jutil ();
      return (0);
    }
  kk_cursor_invisible ();
  end_jutil ();
  if (js_dic_info (cur_env, dicinfo[cc].dic_no, &dic) < 0)
    {
      if_dead_disconnect (cur_env, -1);
      return (0);
    }
  if (js_file_info (cur_env, dic.body, &file) < 0)
    {
      if_dead_disconnect (cur_env, -1);
      return (0);
    }
  if (js_file_comment_set (cur_env, file.fid, com) == -1)
    {
      if_dead_disconnect (cur_env, -1);
      errorkeyin ();
      return (0);
    }
  return (1);
}

/*
 *
 *  Functions Called during select element (dic_entries).
 *
 */

/* Defined in kensaku.c */
extern struct wnn_jdata *word_searched;
extern int del_point, ima_point, hindo_point, com_point, max_c_len;

int
kworddel ()
{
  int type;

  if (cur_root->ichi->buf[cc][del_point] == 'D')
    {
      change_ichi_buf (cc, del_point, " ");
      return (0);
    }
  type = dicinfo[find_dic_by_no (word_searched[cc].dic_no)].type;
  if (type != WNN_UD_DICT && type != WNN_REV_DICT && type != CWNN_REV_DICT)
    {
      print_msg_getc (msg_get (cd, 82, default_message[82], xjutil->lang), NULL, NULL, NULL);
      return (0);
    }
  if (dicinfo[find_dic_by_no (word_searched[cc].dic_no)].rw == WNN_DIC_RDONLY)
    {
      print_msg_getc (msg_get (cd, 83, default_message[83], xjutil->lang), NULL, NULL, NULL);
      return (0);
    }
  change_ichi_buf (cc, del_point, "D");
  return (0);
}

int
kworddel_op ()
{
  register int i;
  char buf[512];
  Ichiran *ichi = cur_root->ichi;

  for (i = 0; i < ichi->kosuu_all; i++)
    {
      if (ichi->buf[i][del_point] != 'D')
        continue;
      strcpy (buf, msg_get (cd, 84, default_message[84], xjutil->lang));
      sStrcpy (buf + strlen (buf), word_searched[i].kanji);
      sprintf (buf + strlen (buf), "%s", msg_get (cd, 85, default_message[85], xjutil->lang));
      if (yes_or_no (buf) == 1)
        {
          if (js_word_delete (cur_env, word_searched[i].dic_no, word_searched[i].serial) == -1)
            {
              if_dead_disconnect (cur_env, -1);
              errorkeyin ();
            }
        }
    }
  return (1);
}

int
kwordcom ()
{
  w_char com[512];
  char buf[512];
  int type, len;
  char *mes[1];

  type = dicinfo[find_dic_by_no (word_searched[cc].dic_no)].type;
  if (type != WNN_UD_DICT && type != WNN_REV_DICT && type != CWNN_REV_DICT)
    {
      print_msg_getc (msg_get (cd, 89, default_message[89], xjutil->lang), NULL, NULL, NULL);
      return (0);
    }
  if (dicinfo[find_dic_by_no (word_searched[cc].dic_no)].rw == WNN_DIC_RDONLY)
    {
      print_msg_getc (msg_get (cd, 90, default_message[90], xjutil->lang), NULL, NULL, NULL);
      return (0);
    }

  com[0] = 0;
  mes[0] = msg_get (cd, 81, default_message[81], xjutil->lang);
  init_jutil (msg_get (cd, 39, default_message[39], xjutil->lang), msg_get (cd, 8, default_message[8], xjutil->lang), 1, &mes, NULL);
  cur_text->max_pos = 127;
  clr_line_all ();
  throw_col (0);
  kk_cursor_normal ();
  if (kana_in (UNDER_LINE_MODE, com, 512) == -1)
    {
      kk_cursor_invisible ();
      end_jutil ();
      return (0);
    }
  kk_cursor_invisible ();
  end_jutil ();
  if (js_word_comment_set (cur_env, word_searched[cc].dic_no, word_searched[cc].serial, com) == -1)
    {
      if_dead_disconnect (cur_env, -1);
      errorkeyin ();
      return (0);
    }
  bzero (buf, sizeof (buf));
  if (Strlen (com))
    {
      strcpy (buf, "{");
      sStrcpy (buf + strlen (buf), com);
      strcat (buf, "}  ");
    }
  if (strlen (buf) > max_c_len)
    {
      len = strlen (buf) - max_c_len;
      insert_space_in_ichi_buf (com_point + max_c_len, len);
      max_c_len = strlen (buf);
      ima_point += len;
      hindo_point += len;
    }
  else
    {
      fill_space (buf, max_c_len);
    }
  change_ichi_buf (cc, com_point, buf);
  return (0);
}

static int
call_hindo_set (ima, hindo)
     int ima;
     int hindo;
{
  if (js_hindo_set (cur_env, word_searched[cc].dic_no, word_searched[cc].serial, ima, hindo) == -1)
    {
      if_dead_disconnect (cur_env, -1);
      errorkeyin ();
      return (0);
    }
  return (1);
}

int
kwordima ()
{
  if (word_searched[cc].ima)
    {
      if (call_hindo_set (WNN_IMA_OFF, WNN_HINDO_NOP))
        {
          change_ichi_buf (cc, ima_point, " ");
          word_searched[cc].ima = 0;
        }
    }
  else
    {
      if (call_hindo_set (WNN_IMA_ON, WNN_HINDO_NOP))
        {
          change_ichi_buf (cc, ima_point, "*");
          word_searched[cc].ima = 1;
        }
    }
  return (0);
}

int
kworduse ()
{
  int ima, hindo;
  char buf[32];
  extern int wnn_word_use_initial_hindo;

  if (word_searched[cc].hindo != -1)
    {
      if (call_hindo_set (WNN_HINDO_NOP, WNN_ENTRY_NO_USE))
        {
          change_ichi_buf (cc, ima_point, "----  ");
          word_searched[cc].hindo = -1;
        }
    }
  else
    {
      ima = wnn_word_use_initial_hindo & 0x80;
      hindo = wnn_word_use_initial_hindo & 0x7f;
      if (call_hindo_set ((ima ? WNN_IMA_ON : WNN_IMA_OFF), hindo))
        {
          sprintf (buf, "%c%-3d  ", (ima ? '*' : ' '), hindo);
          change_ichi_buf (cc, ima_point, buf);
          word_searched[cc].hindo = hindo;
          word_searched[cc].ima = ima;
        }

    }
  return (0);
}

int
kwordhindo ()
{
  int cur_hindo;
  char *mes[1];
  w_char kana_buf[512];
  char st[32];
  char buf[32];
  int newhindo;
  char message[80];

  if (dicinfo[find_dic_by_no (word_searched[cc].dic_no)].hindo_rw == WNN_DIC_RDONLY)
    {
      print_msg_getc (msg_get (cd, 118, default_message[118], xjutil->lang), NULL, NULL, NULL);
      return (0);
    }
  cur_hindo = word_searched[cc].hindo;

  sprintf (message, "%s %d %s", msg_get (cd, 119, default_message[119], xjutil->lang), cur_hindo, msg_get (cd, 32, default_message[32], xjutil->lang));

  mes[0] = msg_get (cd, 33, default_message[33], xjutil->lang);
  init_jutil (message, msg_get (cd, 8, default_message[8], xjutil->lang), 1, &mes, NULL);
Retry:
  cur_text->max_pos = 127;
  clr_line_all ();
  throw_col (0);
  kk_cursor_normal ();
  kana_buf[0] = 0;
  if (kana_in (UNDER_LINE_MODE, kana_buf, 512) == -1)
    {
      kk_cursor_invisible ();
      end_jutil ();
      return (0);
    }
  kk_cursor_invisible ();
  if (sStrcpy_and_ck (st, kana_buf) || sscanf (st, "%d", &newhindo) <= 0)
    {
      print_msg_getc ("%s", msg_get (cd, 34, default_message[34], xjutil->lang), NULL, NULL);
      goto Retry;
    }
  else if (newhindo < 0)
    {
      print_msg_getc ("%s", msg_get (cd, 35, default_message[35], xjutil->lang), NULL, NULL);
      goto Retry;
    }

  end_jutil ();
  if (call_hindo_set (WNN_HINDO_NOP, newhindo))
    {
      sprintf (buf, "%-3d  ", newhindo);
      change_ichi_buf (cc, hindo_point, buf);
      word_searched[cc].hindo = newhindo;
    }
  return (0);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
