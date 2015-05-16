/*
 * $Id: hinsi.c,v 1.4.2.1 1999/02/08 07:43:32 yamasita Exp $
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
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

#define ROOT "/"

#define MAXHINSI 256

int save_k[5];
int save_k_p;
char *bunpou_title;
int save_t_p;
char *save_t[10];

static w_char *
default_bunpou_search (node, total)
     w_char *node, *total;
{
  int cnt;
  w_char **whbuf;
  w_char *ret;

  if (node == NULL)
    return (NULL);
  if ((cnt = js_hinsi_list (cur_env, -1, node, &rb)) == -1)
    if_dead_disconnect (cur_env, -1);
  if (cnt == -1)
    {
      errorkeyin ();
      return (NULL);
    }
  whbuf = (w_char **) (rb.buf);
  if (cnt == 0)
    return (node);
  Strcpy (node, whbuf[0]);
  Strcat (total, node);
  ret = default_bunpou_search (node, total);
  return (ret);
}

int
get_default_hinsi (p)
     char *p;
{
  w_char *a;
  w_char tmp[WNN_HINSI_NAME_LEN];
  w_char total[WNN_HINSI_NAME_LEN];
  register int x;

  Sstrcpy (tmp, ROOT);
  Sstrcpy (total, ROOT);

  if ((a = default_bunpou_search (tmp, total)) == NULL)
    {
      return (-1);
    }
  if ((x = js_hinsi_number (cur_env->js_id, a)) == -1)
    if_dead_disconnect (cur_env, -1);
  sStrcpy (p, total);
  return (x);
}

static w_char *
bunpou_search (node, total)
     w_char *node, *total;
{
  int cnt;
  int k;
  char *hbuf[MAXHINSI];
  char buf[1024];               /* iikagen */
  char *c;
  w_char **whbuf;
  w_char *ret;

  if (node == NULL)
    return (NULL);
  if ((cnt = js_hinsi_list (cur_env, -1, node, &rb)) == -1)
    if_dead_disconnect (cur_env, -1);
  if (cnt == -1)
    {
      errorkeyin ();
      return (NULL);
    }
  whbuf = (w_char **) (rb.buf);
  if (cnt == 0)
    return (node);
  for (k = 0, c = buf; k < cnt; k++)
    {
      hbuf[k] = c;
      sStrcpy (c, whbuf[k]);
      c += strlen (c) + 1;
    }
  hbuf[cnt] = msg_get (cd, 0, default_message[0], xjutil->lang);
TOP:
  k = xw_select_one_element_call (hbuf, cnt + 1, 0, bunpou_title);
  if (k == -1 || k == -3)
    return (NULL);
  if (strcmp (hbuf[k], hbuf[cnt]) == 0)
    {
      JWMline_clear (save_k[--save_k_p]);
      bunpou_title = save_t[--save_t_p];
      return ((w_char *) hbuf[cnt]);
    }
  Sstrcpy (node, hbuf[k]);
  Strcat (total, node);
  save_k[save_k_p++] = cur_text->currentcol;
  xw_jutil_write_msg (hbuf[k]);
  save_t[save_t_p++] = bunpou_title;
  bunpou_title = hbuf[k];
  ret = bunpou_search (node, total);
  if (ret == NULL)
    return (NULL);
  if (strcmp ((char *) ret, hbuf[cnt]) == 0)
    goto TOP;
  return (ret);
}

int
hinsi_in (text)
     unsigned char *text;
{
  w_char *a;
  register int x;
  w_char tmp[WNN_HINSI_NAME_LEN];
  w_char total[WNN_HINSI_NAME_LEN];

  Sstrcpy (tmp, ROOT);
  Sstrcpy (total, ROOT);
  bunpou_title = msg_get (cd, 1, default_message[1], xjutil->lang);
  save_k_p = 0;
  save_t_p = 0;
  save_t[0] = bunpou_title;

  if ((a = bunpou_search (tmp, total)) == NULL || strcmp ((char *) a, msg_get (cd, 0, default_message[0], xjutil->lang)) == 0)
    {
      return (-1);
    }
  if ((x = js_hinsi_number (cur_env->js_id, a)) == -1)
    {
      if_dead_disconnect (cur_env, -1);
    }
  else
    {
      sStrcpy (text, total);
    }
  return (x);
}
