/*
 * $Id: xresource.c,v 1.5.2.1 1999/02/08 06:02:59 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
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

/*
 * X11R5 Input Method Test Program
 * xresorce.c v 1.0   Mon Mar  4 10:33:36 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 *
 *              Shoji kamada     OMRON Corporation
 *                               kamada@ari.ncl.omron.co.jp
 */

#include "exvalue.h"
#include "func.h"

#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>

void
createxres ()                   /* CREATE X RESources */
{
  char **miss[ARRAY_MAX], *def[ARRAY_MAX];
  int i[ARRAY_MAX], cnt, pix_no;
  int flags = { DoRed | DoGreen | DoBlue };
  u_long plane[PLANE_MAX];
  static char *name[ARRAY_MAX][PIX_MAX] = {
    {"white", "orange", "lightblue", "forestgreen"},
    {"red", "blue", "yellow", "black"}
  };
  static char *font[ARRAY_MAX] = { "a14, k14, kana14", "-*--18-*" };
  static u_int shape[ARRAY_MAX] = { XC_gumby, XC_trek };
  Visual *visual;
  u_long valuemask;
  XGCValues values;

  prstatus ("Creating X resources...");
  if ((dpy = XOpenDisplay ("")) == NULL)
    {
      cls (stdisp);
      cls (prdisp);
      prprint ("Error : Can't open display\n\n");
      prprint ("Check environment valuable \"DISPLAY\".\n\n");
      freemwin ();
    }
  scr = XDefaultScreen (dpy);
  rwin = RootWindow (dpy, scr);
  visual = DefaultVisual (dpy, scr);
  dcmap = DefaultColormap (dpy, scr);
  if (XAllocColorCells (dpy, dcmap, False, plane, 0, pixel[cmflag], PIX_MAX))
    {
      for (pix_no = 0; pix_no < PIX_MAX; pix_no++)
        {
          XStoreNamedColor (dpy, dcmap, name[cmflag][pix_no], pixel[cmflag][pix_no], flags);
        }
    }
  else
    {
      prprint ("I cannot allocate colors.\n");
      prprint ("I will shutdown this program soon.\n\n");
      freemwin ();
    }
  cmap[0] = XCreateColormap (dpy, rwin, visual, AllocNone);
  cmap[1] = XCreateColormap (dpy, rwin, visual, AllocNone);
  for (cnt = 0; cnt < ARRAY_MAX; cnt++)
    {
      if (XAllocColorCells (dpy, cmap[cnt], False, plane, 0, pixel[cnt], PIX_MAX))
        {
          for (pix_no = 0; pix_no < PIX_MAX; pix_no++)
            {
              XStoreNamedColor (dpy, cmap[cnt], name[cnt][pix_no], pixel[cnt][pix_no], flags);
            }
        }
      else
        {
          prprint ("I cannot allocate colors.\n");
          prprint ("I will shutdown this program soon.\n\n");
          freemwin ();
        }
      fontset[cnt] = XCreateFontSet (dpy, font[cnt], &miss[cnt], &i[cnt], &def[cnt]);
      cursor[cnt] = XCreateFontCursor (dpy, shape[cnt]);
      pix[cnt] = XCreatePixmapFromBitmapData (dpy, rwin, bit_bits[cnt], BIT_WIDTH, BIT_HEIGHT, pixel[cmflag][1], pixel[cmflag][2], DefaultDepth (dpy, scr));
    }
  valuemask = GCFunction | GCForeground | GCBackground;
  values.function = GXcopy;
  values.foreground = pixel[cmflag][2];
  values.background = pixel[cmflag][1];
  gc = XCreateGC (dpy, rwin, valuemask, &values);
  mktwin ();
  mkmwin ();
  mapwin ();
  dstrwin ();
  prstatus ("done.");
}


void
mktwin ()                       /* MaKe Test WINdow */
{
  XSizeHints hints;
  XClassHint class_hints;
  XWMHints wm_hints;
  XTextProperty win_name, icon_name;

  win_name.value = (u_char *) "X11R5 XIM Test";
  win_name.encoding = XA_STRING;
  win_name.format = 8;
  win_name.nitems = strlen (win_name.value);
  icon_name.value = win_name.value;
  icon_name.encoding = XA_STRING;
  icon_name.format = 8;
  icon_name.nitems = win_name.nitems;
  hints.flags = (USPosition | USSize | PMinSize | PMaxSize | PResizeInc | PBaseSize | PWinGravity);
  hints.x = W1_X;
  hints.y = W1_Y;
  hints.width = hints.min_width = hints.max_width = W1_WIDTH;
  hints.height = hints.min_height = hints.min_height = W1_HEIGHT;
  hints.width_inc = hints.height_inc = 0;
  hints.base_width = W1_WIDTH;
  hints.base_height = W1_HEIGHT;
  hints.win_gravity = CenterGravity;
  class_hints.res_name = NULL;
  class_hints.res_class = NULL;
  wm_hints.flags = InputHint;
  wm_hints.input = True;

  window1.win = XCreateSimpleWindow (dpy, rwin, W1_X, W1_Y, W1_WIDTH, W1_HEIGHT, W_BORDER, pixel[cmflag][2], pixel[cmflag][1]);
  window2.win = XCreateSimpleWindow (dpy, window1.win, W23_X, W2_Y, W2_WIDTH, W23_HEIGHT, W_BORDER, pixel[cmflag][2], pixel[cmflag][1]);
  window3.win = XCreateSimpleWindow (dpy, window1.win, W23_X, W3_Y, W3_WIDTH, W23_HEIGHT, W_BORDER, pixel[cmflag][2], pixel[cmflag][1]);
  ufocus = window3.win;
  window1.name = XCreateSimpleWindow (dpy, window1.win, WIN_ITV, CALCY (W1_HEIGHT, NW_HEIGHT), NW_WIDTH, NW_HEIGHT, W_BORDER, pixel[cmflag][1], pixel[cmflag][1]);
  window2.name = XCreateSimpleWindow (dpy, window2.win, WIN_ITV, CALCY (W23_HEIGHT, NW_HEIGHT), NW_WIDTH, NW_HEIGHT, W_BORDER, pixel[cmflag][1], pixel[cmflag][1]);
  window3.name = XCreateSimpleWindow (dpy, window3.win, WIN_ITV, CALCY (W23_HEIGHT, NW_HEIGHT), NW_WIDTH, NW_HEIGHT, W_BORDER, pixel[cmflag][1], pixel[cmflag][1]);
  XSetWMProperties (dpy, window1.win, &win_name, &icon_name, NULL, 0, &hints, &wm_hints, &class_hints);
  XSelectInput (dpy, window1.win, ExposureMask | ButtonPressMask | KeyPressMask);
  XSelectInput (dpy, window1.name, ExposureMask);
  XSelectInput (dpy, window2.win, ExposureMask | ButtonPressMask | KeyPressMask);
  XSelectInput (dpy, window2.name, ExposureMask);
  XSelectInput (dpy, window3.win, ExposureMask | ButtonPressMask | KeyPressMask);
  XSelectInput (dpy, window3.name, ExposureMask);
  XSetWindowColormap (dpy, window1.win, cmap[0]);
  XSetWindowColormap (dpy, window2.win, cmap[0]);
  XSetWindowColormap (dpy, window3.win, cmap[0]);
}


void
mkmwin ()                       /* MaKe Menu WINdow */
{
  int cnt;
  u_int mroot_width;
  u_int mroot_height;
  XSizeHints hints;
  XClassHint class_hints;
  XWMHints wm_hints;
  XTextProperty win_name, icon_name;

  for (cnt = 0; ftbl[cnt].mname != NULL; cnt++)
    ;
  mroot_width = MSUB_WIDTH * cnt;
  mroot_height = MSUB_HEIGHT * 2;
  mroot = XCreateSimpleWindow (dpy, rwin, MROOT_X, MROOT_Y, mroot_width, mroot_height, M_BORDER, pixel[cmflag][2], pixel[cmflag][1]);
  XSelectInput (dpy, mroot, ExposureMask);
  win_name.value = (u_char *) "X11R5 XIM Menu";
  win_name.encoding = XA_STRING;
  win_name.format = 8;
  win_name.nitems = strlen (win_name.value);
  icon_name.value = win_name.value;
  icon_name.encoding = XA_STRING;
  icon_name.format = 8;
  icon_name.nitems = win_name.nitems;
  hints.flags = (USPosition | USSize | PMinSize | PMaxSize | PResizeInc | PBaseSize | PWinGravity);
  hints.x = MROOT_X;
  hints.y = MROOT_Y;
  hints.width = hints.min_width = hints.max_width = mroot_width;
  hints.height = hints.min_height = hints.min_height = mroot_height;
  hints.width_inc = hints.height_inc = 0;
  hints.base_width = mroot_width;
  hints.base_height = mroot_height;
  hints.win_gravity = CenterGravity;
  class_hints.res_name = NULL;
  class_hints.res_class = NULL;
  wm_hints.flags = InputHint;
  wm_hints.input = True;

  XSetWMProperties (dpy, mroot, &win_name, &icon_name, NULL, 0, &hints, &wm_hints, &class_hints);
  strflag = MB;
  strmode = XCreateSimpleWindow (dpy, mroot, MSUB_WIDTH * 11, 0, MSUB_WIDTH, MSUB_HEIGHT, 0, pixel[cmflag][2], pixel[cmflag][1]);
  XSelectInput (dpy, strmode, ExposureMask | ButtonPressMask);
  for (cnt = 0; ftbl[cnt].mname != NULL; cnt++)
    {
      mwin[cnt] = XCreateSimpleWindow (dpy, mroot, MSUB_WIDTH * cnt, MSUB_Y, MSUB_WIDTH, MSUB_HEIGHT, M_BORDER, pixel[cmflag][2], pixel[cmflag][1]);
      XSelectInput (dpy, mwin[cnt], ExposureMask | ButtonPressMask);
    }
  XSetWindowColormap (dpy, mroot, cmap[0]);
}

void
mapwin ()                       /* MAP WINdow */
{
  XMapWindow (dpy, window1.win);
  XMapSubwindows (dpy, window1.win);
  XMapSubwindows (dpy, window2.win);
  XMapSubwindows (dpy, window3.win);
  XMapWindow (dpy, mroot);
  XMapSubwindows (dpy, mroot);
}

void
dstrwin ()                      /* Draw STRing on WINdow */
{
  int cnt;
  char *wfont = "-adobe-helvetica-bold-r-normal--*-120-*";

  XSetFont (dpy, gc, XLoadFont (dpy, wfont));
  XDrawString (dpy, window1.name, gc, MSTR_XY, "Window1", strlen ("Window1"));
  XDrawString (dpy, window2.name, gc, MSTR_XY, "Window2", strlen ("Window2"));
  XDrawString (dpy, window3.name, gc, MSTR_XY, "Window3", strlen ("Window3"));
  XDrawString (dpy, mroot, gc, MSTR_XY, "X-Window System X11R5 Xi18n Input Method Test <<XSetICValues Menu>>", strlen ("X-Window System X11R5 Xi18n Input Method Test <<XSetICValues Menu>>"));
  if (strflag == MB)
    {
      XDrawString (dpy, strmode, gc, MSTR_XY, "Mode: MB", strlen ("Mode: MB"));
    }
  else if (strflag == WC)
    {
      XDrawString (dpy, strmode, gc, MSTR_XY, "Mode: WC", strlen ("Mode: WC"));
    }
  for (cnt = 0; ftbl[cnt].mname != NULL; cnt++)
    {
      XDrawString (dpy, *ftbl[cnt].mwin, gc, MSTR_XY, ftbl[cnt].mname, strlen (ftbl[cnt].mname));
    }
  XFlush (dpy);
}

void
cwcolor (win, bg_pixel)         /* Change Window COLOR */
     Window win;
     u_long bg_pixel;
{
  XSetWindowBackground (dpy, win, bg_pixel);
  XClearWindow (dpy, win);
  dstrwin ();
  XFlush (dpy);
}


void
freexres ()                     /* FREE X RESources */
{
  int cnt = 0;

  XFreeGC (dpy, gc);
  XDestroyWindow (dpy, window1.win);
  XDestroyWindow (dpy, mroot);
  XFreeColormap (dpy, dcmap);
  for (cnt = 0; cnt < ARRAY_MAX; cnt++)
    {
      XFreeFontSet (dpy, fontset[cnt]);
      XFreeColormap (dpy, cmap[cnt]);
      XFreePixmap (dpy, pix[cnt]);
      XFreeCursor (dpy, cursor[cnt]);
    }
  XCloseDisplay (dpy);
}
