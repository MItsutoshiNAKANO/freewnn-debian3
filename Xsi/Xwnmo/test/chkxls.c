/*
 * $Id: chkxls.c,v 1.4.2.1 1999/02/08 06:02:53 yamasita Exp $
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
 * chkxls.c v 1.0   Mon Mar 11 14:27:45 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>
#include "exvalue.h"
#include "func.h"

void
xlstr (ic, event)
     XIC ic;
     XEvent *event;
{
  int res;
  int cnt;
  char mbbuf[MAX_BUF];
  wchar_t wcbuf[MAX_BUF];
  KeySym ksret;
  Status stret;

  if (strflag == MB)
    {
      res = XmbLookupString (ic, event, mbbuf, sizeof (mbbuf), &ksret, &stret);
    }
  else if (strflag == WC)
    {
      res = XwcLookupString (ic, event, wcbuf, sizeof (wcbuf), &ksret, &stret);
    }
  cls (prdisp);
  switch (stret)
    {
    case XLookupBoth:
    case XLookupKeySym:
      prprint ("Key Symbol returned.\n");
      fprintf (icfp, "returned Key Symbol : 0x%X\n", ksret);
      if (stret == XLookupKeySym)
        prprint ("\nCheck your own eyes on the test window.\n\n");
      break;
    case XLookupChars:
      prprint ("Character returned.\n");
      prprint ("\nCheck your own eyes on the test window.\n\n");
      for (cnt = 0; cnt < res; cnt++)
        if (strflag == MB)
          {
            fprintf (icfp, "returned character code : 0x%X\n", mbbuf[cnt]);
          }
        else if (strflag == WC)
          {
            fprintf (icfp, "returned character code : 0x%X\n", wcbuf[cnt]);
          }
      fprintf (icfp, "\n");
      break;
    case XLookupNone:
      prprint ("returned status : XLookupNone\n\n");
      fprintf (icfp, "returned status : XLookupNone\n\n");
      break;
    case XBufferOverflow:
      prprint ("returned status : XBufferOverflow\n\n");
      fprintf (icfp, "returned status : XBufferOverflow\n\n");
      break;
    default:
      prprint ("returned status : Unknown Value\n\n");
      fprintf (icfp, "returned status : Unknown Value\n\n");
      break;
    }
}
