/*
 * $Id: chkfe.c,v 1.2 2001/06/14 18:16:10 ura Exp $
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
 * chkfe.c v 1.0   Fri Mar  8 09:13:21 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include        "exvalue.h"
#include        "func.h"

/* *INDENT-OFF* */
FLAG
fltev (event)
     XEvent *event;
/* *INDENT-ON* */
{
  Bool res;

/*  prstatus("Calling XFilterEvent()..."); */
  res = XFilterEvent (event, event->xany.window);
/*  prstatus("done."); */
  cls (prdisp);
  if (res == False)
    {
/*      prprint("The result of XFilterEvent() is \"False\".\n");  */
      fprintf (icfp, "The result of XFilterEvent() is \"False\".\n");
      return (NG);
    }
  else
    {
/*      prprint("The result of XFilterEvent() is \"True\".\n");  */
      fprintf (icfp, "The result of XFilterEvent() is \"True\".\n");
      return (OK);
    }
}
