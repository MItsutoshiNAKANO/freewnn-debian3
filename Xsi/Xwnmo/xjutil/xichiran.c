/*
 * $Id: xichiran.c,v 1.1.2.1 1999/02/08 07:43:37 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright 1992 by Massachusetts Institute of Technology
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
#include <string.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

void
resize_text (text)
     register JutilTextRec *text;
{
  XMoveResizeWindow (dpy, text->w, 0, 0, FontWidth * (text->maxcolumns + 1), FontHeight);
  XMoveResizeWindow (dpy, text->wn[0], 0, 0, FontWidth, FontHeight);
  XMoveResizeWindow (dpy, text->wn[1], (text->width - FontWidth), 0, FontWidth, FontHeight);
  XMoveResizeWindow (dpy, text->wn[2], (text->width - FontWidth), 0, FontWidth, FontHeight);
  XSetWindowBackground (dpy, text->w, cur_root->bg);
  XSetWindowBackground (dpy, text->wn[0], cur_root->bg);
  XSetWindowBackground (dpy, text->wn[1], cur_root->bg);
  XSetWindowBackground (dpy, text->wn[2], cur_root->bg);
  XClearWindow (dpy, text->w);
  XClearWindow (dpy, text->wn[0]);
  XClearWindow (dpy, text->wn[1]);
  XClearWindow (dpy, text->wn[2]);
}

void
xw_mousemove (event)
     XMotionEvent *event;
{
  register Ichiran *ichi = cur_root->ichi;

  if (!ichi->lock && xjutil->mode & ICHIRAN_MODE)
    {
      if (ichi->mode == SENTAKU)
        {
          xw_move_hilite (event->y);
        }
      else
        {
          xw_jikouho_move_hilite (event->x, event->y);
        }
    }
}

void
init_keytable (kosuu, title)
     int kosuu;
     unsigned char *title[];
{
  int cnt;
  int len, x, y;
  int width, height, x_cood, y_cood;
  Window root, child;
  int win_x, win_y;
  unsigned int keys_buttons;
  int save_x, save_y;
  Keytable *keytable = cur_root->ichi->keytable;

  keytable->map = 0;

  x = FontWidth * 2;
  y = FontHeight / 2 - IN_BORDER;
  height = FontHeight + IN_BORDER * 2;
  keytable->cnt = kosuu;
  for (cnt = 0; cnt < kosuu; cnt++)
    {
      len = columnlen (title[cnt]);
      keytable->button[cnt]->string = title[cnt];
      moveresize_box (keytable->button[cnt], x, y, (FontWidth * len + IN_BORDER * 2), height);
      map_box (keytable->button[cnt]);
      x += FontWidth * (len + 2) + IN_BORDER * 2;
    }
  width = x;
  height = FontHeight * 2;

  XQueryPointer (dpy, cur_root->root_window, &root, &child, &save_x, &save_y, &win_x, &win_y, &keys_buttons);
  keytable->save_p.x = (short) save_x;
  keytable->save_p.y = (short) save_y;
  decide_position (width, height, cur_root->bw, &x_cood, &y_cood);
  XWarpPointer (dpy, None, cur_root->root_window, 0, 0, 0, 0, x_cood + width / 2, y_cood + FontHeight);

  XMoveResizeWindow (dpy, keytable->w, x_cood, y_cood, width, height);
  set_wm_properties (keytable->w, x_cood, y_cood, width, height, CenterGravity, NULL, NULL, 0, 0, 0, 0, 0);
  XRaiseWindow (dpy, keytable->w);
  XMapWindow (dpy, keytable->w);
  XFlush (dpy);
}

void
end_keytable ()
{
  int cnt;
  Keytable *keytable = cur_root->ichi->keytable;

  for (cnt = 0; cnt < keytable->cnt; cnt++)
    {
      unmap_box (keytable->button[cnt]);
    }
  XUnmapWindow (dpy, keytable->w);
  if (keytable->save_p.x > -1)
    XWarpPointer (dpy, None, cur_root->root_window, 0, 0, 0, 0, keytable->save_p.x, keytable->save_p.y);
  XFlush (dpy);
  keytable->cnt = 0;
  xjutil->sel_ret = -1;
}

void
xw_expose (event)
     register XExposeEvent *event;
{
  register int i;
  Ichiran *ichi = cur_root->ichi;
  JutilRec *jutil = cur_root->jutil;
  Window window = event->window;
  BoxRec *p;

  for (p = box_list; p != NULL; p = p->next)
    {
      if (window == p->window)
        {
          redraw_box (p);
          return;
        }
    }
  if (xjutil->mode & ICHIRAN_MODE)
    {
      if (window == ichi->w1)
        {
          redraw_ichi_w ();
          return;
        }
      else if (window == ichi->nyuu_w)
        {
          JW3Mputc (ichi->nyuu, ichi->nyuu_w, 0, 1, 0);
          return;
        }
    }
  if (xjutil->mode & JUTIL_MODE)
    {
      for (i = 0; i < jutil->max_mes; i++)
        {
          if (jutil->mes_text[i]->w == window)
            {
              redraw_text (jutil->mes_text[i], event->x, event->width);
              return;
            }
          else if (jutil->mes_text[i]->wn[0] == window)
            {
              redraw_note (jutil->mes_text[i], 0);
              return;
            }
          else if (jutil->mes_text[i]->wn[1] == window)
            {
              redraw_note (jutil->mes_text[i], 1);
              return;
            }
        }
    }
  return;
}

/* *INDENT-OFF* */
Status
xw_buttonpress (event)
     XButtonEvent *event;
/* *INDENT-ON* */

{
  Ichiran *ichi = cur_root->ichi;
  Window window = event->window;
  BoxRec *p;

  xjutil->sel_ret = -1;
  if (xjutil->mode & JUTIL_MODE)
    {
      p = cur_root->jutil->button[EXEC_W];
      if (window == p->window)
        {
          xjutil->sel_ret = p->sel_ret;
          if (p->cb)
            (*p->cb) (p->cb_data);
          return (p->do_ret);
        }
    }
  for (p = box_list; p != NULL; p = p->next)
    {
      if (window == p->window)
        {
          if (p->freeze)
            return (False);
          xjutil->sel_ret = p->sel_ret;
          if (p->cb)
            (*p->cb) (p->cb_data);
          return (p->do_ret);
        }
    }

  if (xjutil->mode & ICHIRAN_MODE)
    {
      if (window == ichi->w1 && !ichi->lock)
        {
          xjutil->sel_ret = 1;
          xw_select_button (event);
          return (True);
        }
    }
  return (False);
}

int
jutil_mode_set (mes)
     int mes;
{
  cur_root->jutil->mode = mes;
  return (0);
}

void
xw_enterleave (event, el)
     XCrossingEvent *event;
     char el;
{
  register Ichiran *ichi = cur_root->ichi;
  register Window window = event->window;
  register BoxRec *p;
/*
    register int i;
    register BoxRec *pp;
    register JutilRec *jutil = cur_root->jutil;

    if (xjutil->mode & JUTIL_MODE && jutil->max_mes > 1) {
        for (i = 0; i < jutil->max_mes; i++) {
            p = jutil->mes_button[i];
            if (window == p->window) {
                if (p->freeze) return;
                if (el && !p->in) {
                    changecolor_box(p, 0, 0, cur_root->fg, BoxBorderPixel);
                    for (i = 0; i < jutil->max_mes; i++) {
                        pp = jutil->mes_button[i];
                        if (pp != p && pp->in) {
                            changecolor_box(pp, 0, 0, cur_root->bg,
                                            BoxBorderPixel);
                            pp->in = (char)0;
                        }
                    }
                    p->in = (char)1;
                }
                return;
            }
        }
    }
*/
  for (p = box_list; p != NULL; p = p->next)
    {
      if (window == p->window)
        {
          if (p->freeze)
            return;
          if (p->reverse && el != p->in)
            {
              reverse_box (p, cur_root->invertgc);
              p->in = el;
              return;
            }
        }
    }

  if (xjutil->mode & ICHIRAN_MODE)
    {
      if (window == ichi->w1 && !ichi->lock)
        {
          if (el)
            {
              xw_mousemove (event);
            }
          else
            {
              xw_mouseleave ();
            }
        }
    }
  return;
}
