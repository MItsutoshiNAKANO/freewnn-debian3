/*
 * $Id: evdispatch.c,v 1.18.2.3 1999/04/19 06:10:56 nakanisi Exp $
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
 *      '99/04/19       TAOKA Satoshi - 田岡 智志<taoka@infonets.hiroshima-u.ac.jp>
 *              kill()、getpid() の宣言をコメントアウト。
 *
 * Last modified date: 19,Apr.1999
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

#ifdef BC_X11R5
#define IS_NOT_FROM_XIMR5(ev)   ((ev).xselectionrequest.selection == server_id)

int ximr5_client = 1;           /* True when XIM R5 client comes */
extern Atom server_id;          /* IM server of X11R6 */
#endif /* BC_X11R5 */

static void
keyboard_mapping (ev)
     XMappingEvent *ev;
{
  XRefreshKeyboardMapping (ev);
}

static int
motif_event (ev)
     XClientMessageEvent *ev;
{
  /* extern int kill(), getpid(); */

  if (ev->window == xim->root_pointer[xim->default_screen]->ximclient->w)
    {
      do_end ();
    }
  else if (xim->j_c && (ev->window == xim->cur_j_c_root->ichi->w || ev->window == xim->cur_j_c_root->inspect->w))
    {
      xim->sel_ret = -2;
      cur_cl_change3 (xim->j_c);
      return (0);
    }
  return (1);
}


static char *syuuryou_title = "X INPUT MANAGER";
static char *syuuryou_syuuryou = " EXIT ";
static void
syuuryou_menu (in)
     int in;
{
  static char *buf[1];
  int c;

  cur_x = xim->root_pointer[xim->default_screen]->ximclient;
  cur_p = cur_x;
  cur_lang = cur_p->cur_xl->lang_db;
  c_c = cur_p->cur_xl->w_c;
  cur_rk = c_c->rk;
  cur_rk_table = cur_rk->rk_table;
  cur_input = 0;
  xim->exit_menu_flg = (char) 1;
  buf[0] = syuuryou_syuuryou;
  c = xw_select_one_element (buf, 1, -1, syuuryou_title, SENTAKU, main_table[4], in);
  if (c == 0)
    {
      do_end ();
    }
  else if (c == BUFFER_IN_CONT)
    {
      return;
    }
  xim->exit_menu_flg = (char) 0;
  return;
}


static void
search_expose (event)
     register XExposeEvent *event;
{
  register XIMClientRec *xc;
  register XIMLangRec *xl;
  register BoxRec *p;
  register Window window = event->window;

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (IsPreeditArea (xc) || IsPreeditPosition (xc))
        {
          cur_p = cur_x = xc;
          xl = cur_p->cur_xl;
          cur_lang = xl->lang_db;
          if (window == xl->w[0])
            {
              if (IsPreeditArea (cur_x))
                {
                  redraw_window3 (event->x, event->width);
                  return;
                }
              else if (IsPreeditPosition (cur_x))
                {
                  redraw_lines (event->x, event->width, 1);
                  return;
                }
            }
          else if (window == xl->w[1])
            {
              redraw_lines (event->x, event->width, 2);
              return;
            }
          else if (window == xl->w[2])
            {
              redraw_lines (event->x, event->width, 3);
              return;
            }
          else if (window == xl->wn[0])
            {
              redraw_note (0);
              return;
            }
          else if (window == xl->wn[1])
            {
              redraw_note (1);
              return;
            }
        }

      if (IsStatusArea (xc))
        {
          if (window == cur_p->cur_xl->ws)
            {
              redraw_window0 ();
              return;
            }
        }
    }
  for (p = box_list; p != NULL; p = p->next)
    {
      if (window == p->window)
        {
          if (p->redraw_cb)
            (p->redraw_cb) (p->redraw_cb_data);
          redraw_box (p);
          return;
        }
    }
  if (xim->j_c)
    {
      cur_cl_change3 (xim->j_c);
      if (window == xim->cur_j_c_root->ichi->w1)
        {
          redraw_ichi_w ();
          return;
        }
      else if (window == xim->cur_j_c_root->ichi->nyuu_w)
        {
          JW3Mputc (xim->cur_j_c_root->ichi->nyuu, xim->cur_j_c_root->ichi->nyuu_w, 0, 1, 0);
          return;
        }
      if (window == xim->cur_j_c_root->inspect->w1)
        {
          JW3Mputc (xim->cur_j_c_root->inspect->msg, xim->cur_j_c_root->inspect->w1, 0, 0, IN_BORDER);
          return;
        }
    }
  return;
}

static void
xw_expose (event)
     register XExposeEvent *event;
{
  register XIMClientRec *cur_x_sv, *cur_p_sv;
  register XIMLangDataBase *cur_lang_sv;

  cur_x_sv = cur_x;
  cur_p_sv = cur_p;
  cur_lang_sv = cur_lang;
  search_expose (event);
  cur_p = cur_p_sv;
  cur_x = cur_x_sv;
  cur_lang = cur_lang_sv;
}

static Status
xw_buttonpress (event)
     XButtonEvent *event;
{
  register XIMClientRec *xc;
  register BoxRec *p;
  register Window window = event->window;

  xim->sel_ret = -1;
  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (IsPreeditNothing (xc))
        continue;
      if (window == xc->cur_xl->ws)
        {
          if (xim->j_c == NULL)
            {
              xim->sel_button = 1;
              cur_cl_change3 (xc);
              if (event->button == Button1)
                {
                  jutil_c ();
                }
              else
                {
                  lang_c ();
                }
            }
          return (False);
        }
    }

  for (p = box_list; p != NULL; p = p->next)
    {
      if (window == p->window)
        {
          if (p->freeze)
            return (False);
          xim->sel_ret = p->sel_ret;
          if (p->cb)
            (*p->cb) (p->cb_data);
          return (p->do_ret);
        }
    }
  if (xim->j_c)
    {
      if (window == xim->cur_j_c_root->ichi->w1)
        {
          xim->sel_button = 1;
          if (xim->cur_j_c_root->ichi->mode == SENTAKU)
            {
              xw_select_button (event);
            }
          else
            {
              xw_select_jikouho_button (event);
            }
          cur_cl_change3 (xim->j_c);
          return (True);
        }
    }
  return (False);
}

static void
xw_destroy (event)
     register XDestroyWindowEvent *event;
{
  register XIMClientRec *xc;
  register int i;

#ifdef  XJPLIB
  if (XJp_xw_destroy (event))
    return;
#endif /* XJPLIB */
#ifdef  USING_XJUTIL
  if (xjutil_destroy (event))
    return;
#endif /* USING_XJUTIL */
  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (event->window == xc->w)
        {
          for (i = 0; i < xim->screen_count; i++)
            {
              if (xc == xim->root_pointer[i]->ximclient)
                {
                  create_xim_window (xim->root_pointer[i], xc);
                  create_preedit (xc, xc->cur_xl, 1);
                  create_status (xc, xc->cur_xl, 1);
                  XMapWindow (dpy, xc->cur_xl->ws);
                  return;
                }
            }
          destroy_client (xc);
          return;
        }
    }
}

static void
xw_mousemove (event)
     XMotionEvent *event;
{
  if (xim->j_c)
    {
      if (xim->cur_j_c_root->ichi->mode == SENTAKU)
        {
          xw_move_hilite (event->y);
        }
      else
        {
          xw_jikouho_move_hilite (event->x, event->y);
        }
    }
}

static void
xw_enterleave (event, el)
     XCrossingEvent *event;
     char el;
{
  register BoxRec *p;
  register Window window = event->window;

  for (p = box_list; p != NULL; p = p->next)
    {
      if (window == p->window)
        {
          if (!p->freeze && p->reverse && el != p->in)
            {
              reverse_box (p, p->invertgc);
              p->in = el;
            }
          return;
        }
    }
  if (xim->j_c)
    {
      if (window == xim->cur_j_c_root->ichi->w1)
        {
          if (el)
            {
              xw_mousemove (event);
            }
          else
            {
              if (xim->cur_j_c_root->ichi->mode == SENTAKU)
                xw_mouseleave ();
            }
          return;
        }
    }
  return;
}

static void
xw_reparent (event)
     XReparentEvent *event;
{
/*
    register XIMClientRec *xc;

    for (xc = ximclient_list; xc != NULL; xc = xc->next) {
        if (xc->xl[0] && (xc->lang_num > 1)) {
            if (event->window == xc->xl[0]->wp[0]) {
                reparent_preedit(xc, event->parent);
                return;
            }
            if (event->window == xc->xl[0]->ws) {
                reparent_status(xc, event->parent);
                return;
            }
        }
    }
*/
  read_wm_id ();
}

static void
xw_configure (ev)
     XConfigureEvent *ev;
{
  register XIMClientRec *xc;

  for (xc = ximclient_list; xc != NULL; xc = xc->next)
    {
      if (xc->w == ev->window)
        {
          change_client_area (xc, ev->x, ev->y, ev->width, ev->height);
          return;
        }
      else if (xc->focus_window == ev->window)
        {
          change_focus_area (xc, ev->x, ev->y, ev->width, ev->height);
          return;
        }
    }
}

void
X_flush ()
{
  if (cur_x && cur_p && IsPreeditPosition (cur_x) && cur_p->cur_xl->del_x >= cur_p->cur_xl->max_pos)
    {
      JWMline_clear1 ();
    }
  XFlush (dpy);
}

static void
kill_xim (window, prop_id, force)
     Window window;
     Atom prop_id;
     int force;
{
  XEvent event;
  register int i, ok = 0;
  register XIMClientRec *xc;
  unsigned char *data;
  Atom actual_type;
  int actual_format;
  unsigned long nitems, leftover;

  for (i = 0, xc = ximclient_list; xc; xc = xc->next)
    i++;
#ifdef  XJPLIB
  i += XJp_get_client_cnt ();
#endif /* XJPLIB */
  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = xim->root_pointer[xim->default_screen]->ximclient->w;
  event.xclient.data.l[0] = XWNMO_KILL;
  if (prop_id)
    {
      XGetWindowProperty (dpy, xim->root_pointer[xim->default_screen]->root_window, prop_id, 0L, 1000000L, False, XA_STRING, &actual_type, &actual_format, &nitems, &leftover, &data);
      if (nitems > 0 && data && *data && !strncmp (root_username, (char *) data, nitems))
        {
          if (force || i == 1)
            {
              ok = 1;
              event.xclient.data.l[2] = 0;
            }
          else
            {
              event.xclient.data.l[2] = i - 1;
            }
        }
      else
        {
          event.xclient.data.l[2] = -1;
        }
      XFree (data);
    }
  else
    {
      event.xclient.data.l[2] = -1;
    }
  if (ok)
    {
      event.xclient.data.l[1] = XWNMO_KILL_OK;
    }
  else
    {
      event.xclient.data.l[1] = XWNMO_KILL_NG;
    }
  XSendEvent (dpy, window, False, NoEventMask, &event);
  XFlush (dpy);
  if (ok)
    {
      do_end ();
    }
}

static Bool
dummy_proc (display, ev, arg)
     Display *display;
     XEvent *ev;
     char *arg;
{
  return (True);
}

void
XEventDispatch ()
{
  int buff[32], in;
  XEvent event;
  int ret = 0, n_bytes;
  register int i;
  extern Atom wm_id;
  extern Atom wm_id1;

/*
    while (XPending(dpy) > 0) {
*/
  do
    {
/*
        XNextEvent(dpy, &event);
        if (XCheckMaskEvent(dpy, ~NoEventMask, &event) == False) return;
*/
      if (XCheckIfEvent (dpy, &event, dummy_proc, 0) == False)
        return;
      ret = 0;
      switch (event.type)
        {
        case KeyPress:
          xim->sel_button = (char) 0;
          ret = key_input (buff, &event);
          break;
        case Expose:
          xw_expose (&event);
          break;
        case ButtonRelease:
          button.x = event.xbutton.x_root;
          button.y = event.xbutton.y_root;
          if (event.xbutton.window == xim->root_pointer[xim->default_screen]->ximclient->w)
            {
              if (xim->exit_menu)
                {
                  xim->sel_button = 1;
                  syuuryou_menu ();
                }
            }
          else
            {
              if (xw_buttonpress (&event) == True)
                {
                  if (xim->exit_menu_flg && xim->j_c && ((event.xbutton.window == xim->cur_j_c_root->ichi->button[CANCEL_W]->window) || (event.xbutton.window == xim->cur_j_c_root->ichi->w1)))
                    {
                      syuuryou_menu ();
                    }
                  else
                    {
                      buff[0] = 0xd;
                      ret = 1;
                    }
                }
            }
          break;
        case DestroyNotify:
          xw_destroy (&event);
          break;
        case MotionNotify:
          xw_mousemove (&event);
          break;
        case EnterNotify:
          xw_enterleave (&event, 1);
          break;
        case LeaveNotify:
          xw_enterleave (&event, 0);
          break;
        case ReparentNotify:
          xw_reparent (&event);
          break;
        case MapNotify:
          check_map (event.xmap.window);
          break;
        case MappingNotify:
          keyboard_mapping (&event);
          break;
        case ConfigureNotify:
          xw_configure (&event);
          break;
        case ClientMessage:
          if (wm_id && (event.xclient.message_type == wm_id) && (event.xclient.data.l[0] == wm_id1))
            {
              motif_event (&event);
              break;
            }
#ifdef  USING_XJUTIL
          if (event.xclient.data.l[0] == DIC_START)
            {
              set_xjutil_id (event.xclient.data.l[1], event.xclient.data.l[2], 0);
            }
          else if (event.xclient.data.l[0] == DIC_START_ER)
            {
              set_xjutil_id (event.xclient.data.l[1], event.xclient.data.l[2], 1);
            }
          else if (event.xclient.data.l[0] == DIC_FONTSET_ER)
            {
              reset_xjutil_fs_id (event.xclient.data.l[1], event.xclient.data.l[2]);
            }
          else if (event.xclient.data.l[0] == DIC_END)
            {
              end_xjutil_work (event.xclient.data.l[1]);
            }
#endif /* USING_XJUTIL */
          if (event.xclient.data.l[0] == XWNMO_KILL)
            {
              kill_xim (event.xclient.data.l[1], event.xclient.data.l[2], event.xclient.data.l[3]);
            }
#ifdef  XJPLIB
          else
            {
              XJp_event_dispatch (&event);
            }
#endif
          break;
#ifndef X11R5
        case SelectionRequest:
#ifdef BC_X11R5
          if (IS_NOT_FROM_XIMR5 (event))
            ximr5_client = 0;
#endif /* BC_X11R5 */
          XimConvertTarget (dpy, (XSelectionRequestEvent *) & event);
          break;
#endif /* !X11R5 */
        default:
          break;
        }
      for (i = 0; i < ret;)
        {
          n_bytes = get_cswidth_by_char (in = buff[i++]);
          for (; n_bytes > 1 && i < ret; n_bytes--, i++)
            {
              in = (in << 8) + buff[i];
            }
          in_put (in);
        }
/*
    }
*/
    }
  while (XPending (dpy) > 0);
}

static Bool
yes_no_proc (display, ev, arg)
     Display *display;
     XEvent *ev;
     char *arg;
{
  register Window w = ev->xany.window;

  if (w == cur_p->yes_no->w || w == cur_p->yes_no->title->window || w == cur_p->yes_no->button[0]->window || w == cur_p->yes_no->button[1]->window)
    {
      return (True);
    }
  return (False);
}

int
get_yes_no_event ()
{
  int buff[16];
  XEvent event;

  while (1)
    {
      XFlush (dpy);
      XIfEvent (dpy, &event, yes_no_proc, 0);
      switch (event.type)
        {
        case KeyPress:
          if (key_input (buff, &event) > 0)
            return (buff[0]);
          break;
        case Expose:
          xw_expose (&event);
          return (0);
        case ButtonRelease:
          button.x = event.xbutton.x_root;
          button.y = event.xbutton.y_root;
          if (xw_buttonpress (&event) == True)
            {
              return (0xd);
            }
          break;
        case EnterNotify:
          xw_enterleave (&event, 1);
          break;
        case LeaveNotify:
          xw_enterleave (&event, 0);
          break;
        case MapNotify:
          check_map (event.xmap.window);
          return (0);
        default:
          break;
        }
    }
}
