/*
 * $Id: xw_touroku.c,v 1.6.2.1 1999/02/08 07:43:39 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
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

#include <stdio.h>
#include <string.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"

void
xw_jutil_write_msg (msg)
     unsigned char *msg;
{
  int wc_len;

  while (1)
    {
      wc_len = char_to_wchar (xjutil->xlc, msg, wc_buf, strlen ((char *) msg), wc_buf_max);
      if (wc_len < -1)
        {
          return;
        }
      else if (wc_len == -1)
        {
          if (realloc_wc_buf () < 0)
            return;
        }
      else
        {
          break;
        }
    }
  JW1Mputwc (wc_buf, wc_len);
  if (cur_text->currentcol > cur_text->max_pos)
    {
      cur_text->max_pos = cur_text->currentcol;
    }
  XFlush (dpy);
}

void
init_jutil (title, exec_t, cnt, t1, mes1)
     unsigned char *title, *exec_t;
     int cnt;
     unsigned char *t1[];
     unsigned char *mes1[];
{
  register JutilRec *jutil = cur_root->jutil;
  register BoxRec *box;
  register JutilTextRec *text;
  int width, height, x_cood, y_cood;
#ifdef nodef
  int save_x, save_y;
  Window root, child;
  int win_x, win_y;
  unsigned int keys_buttons;
#endif
  int i;
  int len_t, max_mes_t;
  int max_columns;

  xjutil->mode |= JUTIL_MODE;

  jutil->title->string = title;
  jutil->button[EXEC_W]->string = exec_t;
  jutil->max_mes = cnt;
  for (i = 0; i < jutil->max_mes; i++)
    jutil->mes_button[i]->string = NULL;

  moveresize_box (jutil->title, FontWidth, FontHeight / 2, (FontWidth * (len_t = strlen ((char *) title)) + IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
  len_t += 1;
  moveresize_box (jutil->button[CANCEL_W],
                  (FontWidth * (len_t += SPACING)), FontHeight / 2, (FontWidth * strlen ((char *) jutil->button[CANCEL_W]->string) + IN_BORDER * 2), FontHeight + IN_BORDER * 2);
  len_t += strlen ((char *) jutil->button[CANCEL_W]->string);
  moveresize_box (jutil->button[EXEC_W], (FontWidth * (len_t += SPACING)), FontHeight / 2, (FontWidth * strlen ((char *) jutil->button[EXEC_W]->string) + IN_BORDER * 2), FontHeight + IN_BORDER * 2);
  XSetWindowBackground (dpy, jutil->w, cur_root->bg);
  changecolor_box (jutil->title, cur_root->fg, cur_root->bg, cur_root->fg, BoxAll);
  changecolor_box (jutil->button[CANCEL_W], cur_root->fg, cur_root->bg, cur_root->fg, BoxAll);
  changecolor_box (jutil->button[EXEC_W], cur_root->fg, cur_root->bg, cur_root->fg, BoxAll);

  jutil->mode = MESSAGE1;
  max_mes_t = 0;
  for (i = 0; i < jutil->max_mes; i++)
    {
      box = jutil->mes_button[i];
      box->string = t1[i];
      if (jutil->max_mes > 1)
        {
          box->cb = jutil_mode_set;
          box->cb_data = (int *) i;
          box->do_ret = True;
        }
      else
        {
          freeze_box (box);
          box->cb = NULL;
          box->cb_data = NULL;
          box->do_ret = False;
        }
      moveresize_box (box, (FontWidth - IN_BORDER), (FontHeight * (3 + i * 2) - IN_BORDER), (FontWidth * (len_t = strlen ((char *) box->string)) + IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
      changecolor_box (box, cur_root->fg, cur_root->bg, cur_root->bg, BoxAll);
      map_box (box);

      len_t += SPACING;
      if (len_t > max_mes_t)
        max_mes_t = len_t;
    }

  width = FontWidth * (TEXT_WIDTH + max_mes_t + SPACING);
  height = FontHeight * (3 + cnt * 2);
  if (width >= DisplayWidth (dpy, cur_root->screen) - cur_root->bw)
    {
      width = DisplayWidth (dpy, cur_root->screen) - cur_root->bw;
      max_columns = width / FontWidth - (max_mes_t + SPACING);
    }
  else
    {
      max_columns = TEXT_WIDTH;
    }

  for (i = 0; i < jutil->max_mes; i++)
    {
      text = jutil->mes_text[i];
      text->max_columns = max_columns;
      text->width = max_columns * FontWidth;
      XMoveResizeWindow (dpy, text->wp, (FontWidth * max_mes_t), (FontHeight * (3 + i * 2)), (FontWidth * text->max_columns), FontHeight);
      XSetWindowBackground (dpy, text->wp, cur_root->bg);
      XClearWindow (dpy, text->wp);
      resize_text (text);
      XMapWindow (dpy, text->wp);
      cur_text = text;
      JWMline_clear (0);
      if (mes1 && mes1[i] && *mes1[i])
        {
          xw_jutil_write_msg (mes1[i]);
        }
    }

  moveresize_box (jutil->rk_mode, (width - FontWidth * (MHL0 + SPACING)), FontHeight / 2, (FontWidth * MHL0 + IN_BORDER * 2), (FontHeight + IN_BORDER * 2));
  changecolor_box (jutil->rk_mode, cur_root->fg, cur_root->bg, cur_root->fg, BoxAll);
  decide_position (width, height, cur_root->bw, &x_cood, &y_cood);
#ifdef nodef
  XQueryPointer (dpy, cur_root->root_window, &root, &child, &save_x, &save_y, &win_x, &win_y, &keys_buttons);
  jutil->save_p.x = (short) save_x;
  jutil->save_p.y = (short) save_y;
  XWarpPointer (dpy, None, cur_root->root_window, 0, 0, 0, 0, x_cood + FontWidth * 2, y_cood + FontHeight);
#endif

  XMoveResizeWindow (dpy, jutil->w, x_cood - 2, y_cood - 2, width, height);
  set_wm_properties (jutil->w, x_cood - 2, y_cood - 2, width, height, CenterGravity, NULL, NULL, 0, 0, 0, 0, 0);
  XRaiseWindow (dpy, jutil->w);
  XMapWindow (dpy, jutil->w);

  disp_mode ();
  XFlush (dpy);

}

static void
clear_jutil_buf (buf)
     int *buf;
{
  register int *x;

  x = buf;
  for (; *x != 0; x++)
    {
      *x = 0;
    }
}

void
end_jutil ()
{
  int i;
  JutilRec *jutil = cur_root->jutil;

  for (i = 0; i < jutil->max_mes; i++)
    {
      clear_jutil_buf (jutil->mes_text[i]->buf);
      unmap_box (jutil->mes_button[i]);
      XUnmapWindow (dpy, jutil->mes_text[i]->wp);
    }
  XWithdrawWindow (dpy, jutil->w, DefaultScreen (dpy));
/*
    XUnmapWindow(dpy, jutil->w);
*/
#ifdef nodef
  if (jutil->save_p.x > -1)
    {
      XWarpPointer (dpy, None, cur_root->root_window, 0, 0, 0, 0, jutil->save_p.x, jutil->save_p.y);
      jutil->save_p.x = jutil->save_p.y = -1;
    }
  XFlush (dpy);
#endif
  jutil->button[EXEC_W]->in = jutil->button[CANCEL_W]->in = 0;
  xjutil->mode &= ~JUTIL_MODE;
  xjutil->sel_ret = -1;
}

void
change_cur_jutil (cur)
     int cur;
{
  cur_text = cur_root->jutil->mes_text[cur];
}

void
hanten_jutil_mes_title (mes, rev)
     int mes;
     int rev;
{
  cur_root->jutil->mes_button[mes]->in = rev;
  redraw_box (cur_root->jutil->mes_button[mes]);
}

void
change_mes_title (mes, msg, rev)
     int mes;
     unsigned char *msg;
     int rev;
{
  cur_root->jutil->mes_button[mes]->string = msg;
  hanten_jutil_mes_title (mes, rev);

}
