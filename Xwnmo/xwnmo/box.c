/*
 * $Id: box.c,v 1.2 2001/06/14 18:16:14 ura Exp $
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
/*      Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */

BoxRec *
create_box (parent, x, y, width, height, bw, fg, bg, bp, cursor, rev)
     Window parent;
     int x, y;
     int width, height;
     int bw;
     unsigned long fg, bg, bp;
     Cursor cursor;
     char rev;
{
  register BoxRec *box;
  XGCValues xgcv;

  if ((box = (BoxRec *) Malloc (sizeof (BoxRec))) == NULL)
    {
      malloc_error ("allocation of box struct");
      return (NULL);
    }
  box->x = x;
  box->y = y;
  box->width = width;
  box->height = height;
  box->border_width = bw;
  box->fg = fg;
  box->bg = bg;
  box->bp = bp;
  box->redraw_cb = NULL;
  box->redraw_cb_data = NULL;
  box->cb = NULL;
  box->cb_data = NULL;
  box->do_ret = False;
  box->sel_ret = -1;
  box->reverse = rev;
  box->in = box->map = '\0';
  box->freeze = '\0';
  box->next = NULL;
  box->window = XCreateSimpleWindow (dpy, parent, (x - bw), (y - bw), width, height, bw, bp, bg);
  xgcv.foreground = fg;
  xgcv.function = GXinvert;
  xgcv.plane_mask = XOR (fg, bg);
  box->invertgc = XCreateGC (dpy, box->window, (GCForeground | GCFunction | GCPlaneMask), &xgcv);
  if (!box->window || !box->invertgc)
    {
      print_out ("Could not create X resources for box.");
      Free ((char *) box);
      return (NULL);
    }

  if (cursor)
    XDefineCursor (dpy, box->window, cursor);
  if (rev)
    {
      XSelectInput (dpy, box->window, (ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | ExposureMask));
    }
  else
    {
      XSelectInput (dpy, box->window, (ButtonPressMask | ButtonReleaseMask | ExposureMask));
    }
  box->next = box_list;
  box_list = box;
  return (box);
}

int
remove_box (b)
     register BoxRec *b;
{
  register BoxRec *p, **prev;
  for (prev = &box_list; p = *prev; prev = &p->next)
    {
      if (p == b)
        {
          *prev = p->next;
          XFreeGC (dpy, p->invertgc);
          Free ((char *) p);
          return (0);
        }
    }
  return (-1);
}

void
map_box (p)
     register BoxRec *p;
{
  XMapWindow (dpy, p->window);
  p->map = 1;
}

void
unmap_box (p)
     register BoxRec *p;
{
  XUnmapWindow (dpy, p->window);
  p->map = '\0';
  p->in = '\0';
}

void
freeze_box (p)
     register BoxRec *p;
{
  p->freeze = 1;
}

void
unfreeze_box (p)
     register BoxRec *p;
{
  p->freeze = '\0';
}

void
moveresize_box (p, x, y, width, height)
     register BoxRec *p;
     int x, y;
     int width, height;
{
  p->x = x;
  p->y = y;
  p->width = width;
  p->height = height;
  XMoveResizeWindow (dpy, p->window, (x - p->border_width), (y - p->border_width), width, height);
}

void
changecolor_box (p, fg, bg, bp, flg)
     register BoxRec *p;
     unsigned long fg, bg, bp;
     unsigned long flg;
{
  XGCValues xgcv;

  if ((flg & BoxBackground) && (bg != p->bg))
    {
      p->bg = bg;
      XSetWindowBackground (dpy, p->window, p->bg);
      xgcv.background = bg;
      xgcv.plane_mask = XOR (p->fg, p->bg);
      XChangeGC (dpy, p->invertgc, (GCPlaneMask | GCBackground), &xgcv);
      XClearWindow (dpy, p->window);
    }
  if ((flg & BoxForeground) && (fg != p->fg))
    {
      p->fg = fg;
      xgcv.foreground = fg;
      xgcv.plane_mask = XOR (p->fg, p->bg);
      XChangeGC (dpy, p->invertgc, (GCPlaneMask | GCForeground), &xgcv);
    }
  if ((flg & BoxBorderPixel) && (bp != p->bp))
    {
      p->bp = bp;
      XSetWindowBorder (dpy, p->window, p->bp);
    }
}

void
redraw_box (p)
     register BoxRec *p;
{
  XClearWindow (dpy, p->window);
/*
    JW3Mputc(p->string, p->window, 0, (int)p->in, IN_BORDER);
*/
  JW3Mputc (p->string, p->window, 0, 0, IN_BORDER);
  if (p->in)
    reverse_box (p, p->invertgc);
}

void
reverse_box (p, invertgc)
     register BoxRec *p;
     GC invertgc;
{
  XFillRectangle (dpy, p->window, invertgc, 0, 0, p->width, p->height);
}

void
change_reverse_box (p, rev)
     register BoxRec *p;
     char rev;
{
  if (rev)
    {
      XSelectInput (dpy, p->window, (ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | ExposureMask));
    }
  else
    {
      XSelectInput (dpy, p->window, (ButtonPressMask | ButtonReleaseMask | ExposureMask));
    }
  p->reverse = rev;
}
