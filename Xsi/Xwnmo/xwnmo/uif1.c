/*
 * $Id: uif1.c,v 1.4.2.1 1999/02/08 08:08:58 yamasita Exp $
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
 *
 * Last modified date: 8,Feb.1999
 *
 * Code:
 *
 */
/*      Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

int
jutil_c (in)
     int in;
{
  static WnnClientRec *c_c_sv = NULL;
  static int jutil_c_step = 0;

  if (c_c_sv != NULL && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
  if (jutil_c_step == 0)
    {
      if (c_c_sv)
        {
          return (-1);
        }
      c_c_sv = c_c;
      push_func (c_c, jutil_c);
      if (!isconect_jserver ())
        {
          c_c_sv = NULL;
          pop_func (c_c);
          return (0);
        }
      jutil_c_step = 1;
    }
  if (jutil (in) == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
  c_c_sv = NULL;
  pop_func (c_c);
  jutil_c_step = 0;
  return (0);
}

int
touroku_c ()
{
  if (!isconect_jserver ())
    {
      return (0);
    }
  touroku ();
  return (0);
}

int
reconnect_jserver_body (in)
     int in;
{
  int k;
  int c;
  static int c_p = 0;
  static WnnClientRec *c_c_sv = 0;
  static int j_con_step = 0;
  static char hostname[32];
  static char *lang;

  if (c_c->use_server == 0)
    return (0);
  if (in == -99)
    {
      end_ichiran ();
      c_c_sv = 0;
      j_con_step = 0;
      romkan_clear ();
      return (-1);
    }
  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
  if (c_c_sv == 0)
    {
      for (k = 0; k < 32; hostname[k++] = '\0');
      c_c_sv = c_c;
      c_p = 0;
      lang = cur_lang->lang;
    }
  if (j_con_step == 0)
    {
      if (init_ichiran ((unsigned char **) NULL, 0, -1,
                        (unsigned char *) msg_get (cd, 29, default_message[29], lang),
                        (unsigned char *) msg_get (cd, 30, default_message[30], lang), (unsigned char *) msg_get (cd, 19, default_message[19], lang), NULL, 32, NYUURYOKU) == -1)
        {
          ring_bell ();
          c_c_sv = 0;
          return (-1);
        }
      sprintf (hostname, "%s", servername);
      c_p = strlen (hostname);
      draw_nyuu_w ((unsigned char *) hostname, 1);
      j_con_step++;
      return (BUFFER_IN_CONT);
    }
  if (j_con_step == 1)
    {
      c = in;
      if (!xim->cur_j_c_root->ichi->map)
        return (BUFFER_IN_CONT);
      if (xim->sel_ret == -2)
        {
          end_ichiran ();
          xim->sel_ret = -1;
          c_c_sv = 0;
          j_con_step = 0;
          romkan_clear ();
          return (-1);
        }
      if (c < 256)
        {
          if ((c == ESC) || (t_quit == main_table[5][c]))
            {
              end_ichiran ();
              c_c_sv = 0;
              romkan_clear ();
              return (-1);
            }
          else if (henkan_off == main_table[5][c])
            {
              ring_bell ();
            }
          else if (c == rubout_code && c_p)
            {
              hostname[--c_p] = '\0';
            }
          else if (c == NEWLINE || c == CR)
            {
              if (c_p == 0)
                {
                  goto RET;
                }
              else
                {
                  end_ichiran ();
                  goto NEXT;
                }
            }
          else if (c > 20 && c < 128)
            {
              hostname[c_p++] = (char) c;
              goto RET;
            }
          else
            {
              ring_bell ();
            }
        }
      else
        {
          ring_bell ();
        }
    RET:
      clear_nyuu_w ();
      draw_nyuu_w ((unsigned char *) hostname, 1);
      return (BUFFER_IN_CONT);
    }
NEXT:
  if (servername)
    Free (servername);
  servername = alloc_and_copy (hostname);

  if (connect_server (cur_lang) < 0)
    goto ERROR_RET;
  if (c_c->use_server && !jl_isconnect (bun_data_))
    {
      print_msg_getc (" %s", msg_get (cd, 32, default_message[32], lang), NULL, NULL);
    }
  else
    {
      print_msg_getc (" %s", msg_get (cd, 33, default_message[33], lang), NULL, NULL);
    }
ERROR_RET:
  c_c_sv = 0;
  j_con_step = 0;
  romkan_clear ();
  return (0);
}

int
lang_c (in)
     int in;
{
  static WnnClientRec *c_c_sv = 0;
  XIMLangRec *xl;
  XIMNestLangRec *p;
  int ret;

  if (in == -99)
    {
      c_c_sv = 0;
      pop_func (c_c);
      return (0);
    }
  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
  if (c_c_sv == 0)
    {
      c_c_sv = c_c;
      push_func (c_c, lang_c);
    }
  if ((ret = lang_set (in, &xl, &p)) == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
  c_c_sv = 0;
  pop_func (c_c);
  if (ret >= 0)
    {
      change_lang (xl, p);
    }
  return (0);
}

int
lang_ct (in)
     int in;
{
  static WnnClientRec *c_c_sv = 0;
  XIMLangRec *xl;
  XIMNestLangRec *p;
  int ret;

  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
  if (c_c_sv == 0)
    {
      c_c_sv = c_c;
      push_func (c_c, lang_ct);
    }
  if ((ret = lang_set_ct (in, &xl, &p)) == BUFFER_IN_CONT)
    {
      return (BUFFER_IN_CONT);
    }
  c_c_sv = 0;
  pop_func (c_c);
  if (ret >= 0)
    {
      change_lang (xl, p);
    }
  return (0);
}

#define UNGETBUFSIZE    32
static unsigned int unget_buf[UNGETBUFSIZE];
static int count = 0;

int
push_unget_buf (c)
     int c;
{
  if ((count + 1) >= UNGETBUFSIZE)
    return (-1);
  unget_buf[count++] = c;
  unget_buf[count] = EOLTTR;
  return (0);
}

unsigned int *
get_unget_buf ()
{
  if (count <= 0)
    unget_buf[0] = EOLTTR;
  count = 0;
  return (unget_buf);
}

int
if_unget_buf ()
{
  if (count > 0)
    return (1);
  return (0);
}
