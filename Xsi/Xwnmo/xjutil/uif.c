/*
 * $Id: uif.c,v 1.7.2.1 1999/02/08 07:43:36 yamasita Exp $
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
#include "rk_spclval.h"         /* defines of CHMSIG, NISEBP */
#include "xext.h"


static int henkan_mode;
/*  0: select
    1: yomi_nyuuryoku
    2: bunsetu_nobasi
    3: kara
*/

static void
change_to_insert_mode ()
{
  c_b->key_table = main_table[1];
  c_b->rk_clear_tbl = romkan_clear_tbl[1];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten = 0x08 | 0x20;
  henkan_mode = 1;
  kk_cursor_normal ();
}

static void
change_to_empty_mode ()
{
  c_b->key_table = main_table[3];
  c_b->rk_clear_tbl = romkan_clear_tbl[3];
  c_b->key_in_fun = NULL;
  c_b->ctrl_code_fun = NULL;
  c_b->hanten = 0x08 | 0x20;
  henkan_mode = 3;
  throw_col (0);
  kk_cursor_normal ();
}

int
empty_modep ()
{
  return (henkan_mode == 3);
}

int
insert_modep ()
{
  return (henkan_mode == 1);
}

int
redraw_nisemono_c ()
{
  redraw_nisemono ();
  if (c_b->maxlen == 0)
    {
      change_to_empty_mode ();
    }
  return (0);
}

int
isconect_jserver ()
{
  if (js_isconnect (cur_env))
    {
      connect_server ();
    }
  if (js_isconnect (cur_env))
    {
      print_msg_getc (" jserver(?)", NULL, NULL, NULL);
      t_print_l ();
      return (0);
    }
  return (1);
}

int
kill_c ()
{
  t_kill ();
  if (c_b->maxlen == 0)
    {
      change_to_empty_mode ();
    }
  return (0);
}

int
delete_c (c, romkan)
     int c, romkan;
{
  t_delete_char (c, romkan);
  if ((c_b->maxlen == 0) && is_HON (romkan))
    {
      change_to_empty_mode ();
    }
  return (0);
}

int
rubout_c (c, romkan)
     int c, romkan;
{
  t_rubout (c, romkan);
  if ((c_b->maxlen == 0) && is_HON (romkan))
    {
      change_to_empty_mode ();
    }
  return (0);
}



int
backward_c ()
{
  if (!isconect_jserver ())
    {
      return (0);
    }
  if (c_b->t_c_p == c_b->t_m_start)
    {
    }
  else
    {
      backward_char ();
    }
  return (0);
}

int
insert_it_as_yomi ()
{
  change_to_insert_mode ();
  c_b->t_m_start = 0;
  cur_bnst_ = 0;
  t_print_l ();
  return (0);
}

int
reconnect_server ()
{
  return (0);
}

int
disconnect_server ()
{
  WnnEnv *p;

  for (p = normal_env; p; p = p->next)
    {
      if (p->env != NULL && js_isconnect (p->env))
        {
          js_close (p->env->js_id);
        }
    }
  return (1);
}
