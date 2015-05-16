/*
 * $Id: keyev.c,v 1.4.2.1 1999/02/08 06:02:57 yamasita Exp $
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
 * keyev.c v 1.0   Thu Mar  7 19:30:17 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"
#include "func.h"

/* *INDENT-OFF* */
FLAG
keyev (ic, mode)                /* transaction of KEY EVent */
    XIC ic;
    FLAG mode;
/* *INDENT-ON* */
{
  int cnt;
  FLAG res;
  FLAG funend;

  XEvent event;

  for (;;)
    {
      XNextEvent (dpy, &event);
      switch (event.type)
        {
        case Expose:
          dstrwin ();
          break;
        case KeyPress:
          if (fltev (&event) == NG)
            xlstr (ic, &event);
          break;
        case ButtonPress:
          if (event.xbutton.window == strmode)
            {
              XClearWindow (dpy, strmode);
              if (strflag == MB)
                {
                  strflag = WC;
                  XDrawString (dpy, strmode, gc, MSTR_XY, "Mode: WC", strlen ("Mode: WC"));
                }
              else if (strflag == WC)
                {
                  strflag = MB;
                  XDrawString (dpy, strmode, gc, MSTR_XY, "Mode: MB", strlen ("Mode: MB"));
                }
              XFlush (dpy);
            }
          else
            {
              funend = NG;
              for (cnt = 0; (ftbl[cnt].mname != NULL) && (funend != OK); cnt++)
                {
                  if (event.xbutton.window == *ftbl[cnt].mwin)
                    {
                      if (ftbl[cnt].flag == CHMOD)
                        {
                          XClearArea (dpy, window1.win, 0, 0, 0, 0, True);
                          return (MENU);
                        }
                      else
                        {
                          cwcolor (*ftbl[cnt].mwin, pixel[cmflag][0]);
                          res = (*ftbl[cnt].func) (ic, mode);
                          cwcolor (*ftbl[cnt].mwin, pixel[cmflag][1]);
                          if (res == COMP)
                            funend = OK;
                          else
                            return (res);
                        }
                    }
                }
              break;
            }
        }
    }
}
