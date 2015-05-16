/*
 * $Id: keyin.c,v 1.13.2.1 1999/02/08 07:43:34 yamasita Exp $
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
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "config.h"
#include "xext.h"

static XGCValues xgcv;
XPoint button;
Atom actual_type;
int actual_format;
unsigned long nitems;
unsigned long leftover;

extern Atom wm_id;
extern Atom wm_id1;

extern char xjutil_env_name[];

static void
change_fore ()
{
  xgcv.foreground = cur_root->fg;
  XChangeGC (dpy, cur_root->gc, GCForeground, &xgcv);
  xgcv.background = cur_root->fg;
  XChangeGC (dpy, cur_root->reversegc, GCBackground, &xgcv);
  xgcv.foreground = cur_root->fg;
  xgcv.plane_mask = XOR (cur_root->fg, cur_root->bg);
  XChangeGC (dpy, cur_root->invertgc, (GCForeground | GCPlaneMask), &xgcv);
}

static void
change_back ()
{
  xgcv.background = cur_root->bg;
  XChangeGC (dpy, cur_root->gc, GCBackground, &xgcv);
  xgcv.foreground = cur_root->bg;
  XChangeGC (dpy, cur_root->reversegc, GCForeground, &xgcv);
  xgcv.background = cur_root->bg;
  xgcv.plane_mask = XOR (cur_root->fg, cur_root->bg);
  XChangeGC (dpy, cur_root->invertgc, (GCBackground | GCPlaneMask), &xgcv);
}

static int
set_xjutil_bun (xjutil_env, p)
     Xjutil_envRec *xjutil_env;
     register char *p;
{
  XFontSet fs;
  XjutilFSRec *fsl;
  int i;
  WnnEnv *we;
#ifdef X11R5
  XLocale xlc;
#else
  XLCd xlc;
#endif /* X11R5 */
  extern XjutilFSRec *add_fontset_list (), *get_fontset_list ();
  extern XFontSet create_font_set ();

  if (!cur_root || cur_root->screen != xjutil_env->screen)
    {
      for (i = 0; i < xjutil->screen_count; i++)
        {
          if (xjutil->root_pointer[i]->screen == xjutil_env->screen)
            {
              cur_root = xjutil->root_pointer[i];
              break;
            }
        }
    }
  if (!cur_root)
    return (-1);
  if (strcmp (xjutil->lc_name, xjutil_env->lc_name))
    {
      xjutil->lc_name = alloc_and_copy (xjutil_env->lc_name);
      xjutil->cswidth_id = xjutil_env->cswidth_id;
      setlocale (LC_ALL, xjutil->lc_name);
#ifdef X11R5
      Free ((char *) xjutil->xlc);
      xlc = _XFallBackConvert ();
      xjutil->xlc = _XlcDupLocale (xlc);
#else
      xlc = _XlcCurrentLC ();
      xjutil->xlc = xlc;
#endif /* X11R5 */
      set_cswidth (xjutil->cswidth_id);
    }
  if ((fsl = get_fontset_list (xjutil_env->fs_id)) == NULL)
    {
      if ((xjutil_env->fn_len <= 0) || (p == NULL) || (*p == '\0'))
        {
          print_out ("XCreateFontSet failed");
          return (-1);
        }
      if ((fs = create_font_set (p)) == NULL)
        {
          return (-1);
        }
      cur_fs = add_fontset_list (fs, xjutil_env->fs_id);
    }
  else
    {
      cur_fs = fsl;
    }
  if (cur_root->fg != xjutil_env->fore_ground)
    {
      cur_root->fg = xjutil_env->fore_ground;
      change_fore ();
    }
  if (cur_root->bg != xjutil_env->back_ground)
    {
      cur_root->bg = xjutil_env->back_ground;
      change_back ();
    }
  for (i = 0, we = normal_env; we; i++, we = we->next)
    {
      if (we->env == NULL)
        continue;
      we->env->env_id = xjutil_env->env_id[i];
      if (xjutil_env->env_id[i] == xjutil_env->cur_env_id)
        cur_normal_env = we;
    }
  for (i = 0, we = reverse_env; we; i++, we = we->next)
    {
      if (we->env == NULL)
        continue;
      we->env->env_id = xjutil_env->env_reverse_id[i];
      if (xjutil_env->env_id[i] == xjutil_env->cur_env_id)
        cur_reverse_env = we;
    }
  env_is_reverse = xjutil_env->env_is_reverse;
  return (0);
}

int
get_env ()
{
  Atom atom_env;
  unsigned char *data;
  Xjutil_envRec xjutil_env;

  if ((atom_env = XInternAtom (dpy, xjutil_env_name, True)) == (Atom) NULL)
    {
      return (-1);
    }
  XGetWindowProperty (dpy, xjutil->root_pointer[xjutil->default_screen]->root_window, atom_env, 0L, 1000000L, False, XA_STRING, &actual_type, &actual_format, &nitems, &leftover, &data);
  bcopy (data, &xjutil_env, sizeof (Xjutil_envRec));
  if (set_xjutil_bun (&xjutil_env, (data + sizeof (Xjutil_envRec))) == -1)
    {
      XFree ((char *) data);
      return (-1);
    }
  XFree ((char *) data);
  return (0);
}

int
get_touroku_data (buffer)
     w_char *buffer;
{
  Atom atom_env;
  unsigned char *data, *data_sv;
  int ret;
  Xjutil_envRec xjutil_env;

  if ((atom_env = XInternAtom (dpy, xjutil_env_name, True)) == (Atom) NULL)
    {
      return (-1);
    }
  XGetWindowProperty (dpy, xjutil->root_pointer[xjutil->default_screen]->root_window, atom_env, 0L, 1000000L, False, XA_STRING, &actual_type, &actual_format, &nitems, &leftover, &data);
  data_sv = data;
  bcopy (data, &xjutil_env, sizeof (Xjutil_envRec));
  if (set_xjutil_bun (&xjutil_env, (data += sizeof (Xjutil_envRec))) == -1)
    {
      XFree (data_sv);
      return (-1);
    }
  data += xjutil_env.fn_len;
  bcopy (data, &ret, sizeof (int));
  data += sizeof (int);
  bcopy (data, buffer, (nitems - (data - data_sv)));
  buffer[(nitems - (data - data_sv)) / 2] = 0;
  XFree (data_sv);
  return (ret);
}

static void
keyboard_mapping (ev)
     XMappingEvent *ev;
{
  XRefreshKeyboardMapping (ev);
}

static int
wm_event (ev)
     XClientMessageEvent *ev;
{
  if (ev->window == cur_root->ichi->w || ev->window == cur_root->jutil->w)
    {
      xjutil->sel_ret = -2;
      return (0);
    }
  return (1);
}

static void
check_map (w)
     Window w;
{
  if (w == cur_root->ichi->w)
    {
      cur_root->ichi->map = 1;
      return;
    }
  else if (w == cur_root->ichi->keytable->w)
    {
      cur_root->ichi->keytable->map = 1;
      return;
    }
  else if (w == cur_root->yes_no->w)
    {
      cur_root->yes_no->map = 1;
      return;
    }
  return;
}

static XComposeStatus compose_status = { NULL, 0 };
static int work = 0;

int
xw_read (buff)
     register int *buff;
{
  XEvent event;
  unsigned char strbuf[32];
  KeySym keycode;
  int nbytes;
  int ck;
  int i;

  for (;;)
    {
      XNextEvent (dpy, &event);
      if (event.type != ClientMessage && !cur_root)
        continue;
      switch (event.type)
        {
        case KeyPress:
          xjutil->sel_button = 0;
          nbytes = XLookupString ((XKeyEvent *) & event, strbuf, 32, &keycode, &compose_status);
          if ((ck = cvt_key_fun (keycode, event.xkey.state)) >= 0)
            {
              *buff++ = ck;
              return (1);
            }
          if ((keycode & 0xff00) == 0xff00)
            {                   /* ファンクションキー */
              if (nbytes > 0)
                {
                  for (i = 0; i < nbytes; i++)
                    {
                      *buff++ = (int) strbuf[i];
                    }
                  return (nbytes);
                }
              else
                {
                  break;
                }
            }
          if (event.xkey.state & ControlMask)
            {                   /* コントロールキー */
              if (*strbuf >= 0x20)
                {
                  break;
                }
              else if (*strbuf == 0x0)
                {
                  *buff++ = (int) *strbuf;
                  return (1);
                }
              else if (*strbuf <= 0x1f)
                {
                  *buff++ = (int) *strbuf;
                  return (1);
                }
            }
          if ((keycode >= 0x4a0) && (keycode <= 0x4df))
            {                   /* 仮名キー */
              *buff++ = (int) ((SS2 << 8) | (keycode & 0xff));
              return (1);
            }
          else if ((keycode < 0x100) && (keycode > 0))
            {
              for (i = 0; i < nbytes; i++)
                {
                  *buff++ = (int) strbuf[i];
                }
              return (nbytes);
            }
          break;

        case ClientMessage:     /* 日本語ツールキットからの要求 */
/* MOTIF */
          if (wm_id && event.xclient.message_type == wm_id && event.xclient.data.l[0] == wm_id1)
            {
              if (wm_event (&event) == 0)
                {
                  *buff = (int) 0xd;
                  return (1);
                }
              break;
            }
/* MOTIF */
          if (event.xclient.data.l[0] == DIC_KILL)
            terminate_handler ();
          if (work)
            break;
          xjutil->sel_button = 0;
          switch (event.xclient.data.l[0])
            {
            case DIC_ADD:
              work = 1;
              jishoadd ();
              send_end_work ();
              work = 0;
              break;
            case DIC_KENSAKU:
              work = 1;
              kensaku ();
              send_end_work ();
              work = 0;
              break;
            case DIC_ICHIRAN:
              work = 1;
              select_one_dict9 ();
              send_end_work ();
              work = 0;
              break;
            case DIC_PARAM:
              work = 1;
              paramchg ();
              send_end_work ();
              work = 0;
              break;
            case DIC_INFO:
              work = 1;
              dicinfoout ();
              send_end_work ();
              work = 0;
              break;
            case DIC_FZK:
              work = 1;
              fuzoku_set ();
              send_end_work ();
              work = 0;
              break;
            case DIC_TOUROKU:
              work = 1;
              touroku ();
              send_end_work ();
              work = 0;
              break;
            default:
              break;
            }

        case Expose:
          xw_expose (&event);
          return (-1);
          break;
        case ButtonRelease:
          button.x = event.xbutton.x_root;
          button.y = event.xbutton.y_root;
          xjutil->sel_button = 1;
          if (xw_buttonpress (&event) == True)
            {
              *buff = (int) 0xd;
              return (1);
            }
          break;
        case DestroyNotify:
          terminate_handler ();
          break;
        case MotionNotify:
          xw_mousemove (&event);
          break;
        case EnterNotify:
          xw_enterleave (&event, (char) 1);
          break;
        case LeaveNotify:
          xw_enterleave (&event, (char) 0);
          break;
        case ReparentNotify:
          read_wm_id ();
          break;
        case MapNotify:
          check_map (event.xmap.window);
          break;
        case MappingNotify:
          keyboard_mapping (&event);
          break;
        default:
          break;

        }
    }
}
