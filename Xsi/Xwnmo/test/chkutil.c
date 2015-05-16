/*
 * $Id: chkutil.c,v 1.2.2.1 1999/02/08 06:02:53 yamasita Exp $
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
 * chkutil.c v 1.0   Thu Mar  7 18:27:37 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>
#include "exvalue.h"
#include "func.h"

void
chkutil (im, ic, locale)        /* CHecK UTILity funcions */
     XIM im;
     XIC ic;
     char *locale;
{
  XIM retim;
  Display *retdpy;
  char *retlocale;
  char tmplocale[MAX_BUF];
  char *cnt;

  cls (prdisp);                 /* clear screen */

  prstatus ("Test of XIMOfIC()...");    /* test of XIMOfIC() */
  retim = XIMOfIC (ic);
  prstatus ("done.");
  if (retim != im)
    {
      prprint ("Test of XIMOfIC() is failed.\n\n");
      fprintf (icfp, "Test of XIMOfIC() is failed.\n");
      fprintf (icfp, "id of im : 0x%-10X, id of retim : 0x%-10X\n\n", im, retim);
    }
  else
    {
      prprint ("Test of XIMOfIC() is succeeded.\n\n");
      fprintf (icfp, "Test of XIMOfIC() is succeeded.\n");
      fprintf (icfp, "id of im : 0x%-10X, id of returned ic : 0x%-10X\n\n", im, retim);
    }

  prstatus ("Test of XDisplayOfIM()...");       /* test of XDisplayOfIM() */
  retdpy = XDisplayOfIM (im);
  prstatus ("done.");
  if (retdpy != dpy)
    {
      prprint ("Test of XDisplayOfIM() is failed.\n\n");
      fprintf (icfp, "Test of XDisplayOfIM() is failed.\n\n");
      fprintf (icfp, "id of Display : 0x%-10X, id of retuened Display : 0x%-10X\n\n", dpy, retdpy);
    }
  else
    {
      prprint ("Test of XDisplayOfIM() is succeeded.\n\n");
      fprintf (icfp, "Test of XDisplayOfIM() is succeeded.\n");
      fprintf (icfp, "id of Display : 0x%-10X, id of returned Display : 0x%-10X\n\n", dpy, retdpy);
    }

  prstatus ("Test of XLocaleOfIM()...");        /* test of XLocaleOfIM() */
  strcpy (tmplocale, locale);
  for (cnt = tmplocale; (*cnt != '\0') && (*cnt != '@'); cnt++)
    ;
  *cnt = '\0';
  retlocale = XLocaleOfIM (im);
  prstatus ("done.");
  if (strcmp (retlocale, tmplocale))
    {
      prprint ("Test of XLocaleOfIM() is failed.\n\n");
      fprintf (icfp, "Test of XLocaleOfIM() is failed.\n");
      fprintf (icfp, "name of locale : %s, name of retuened locale : %s\n\n", tmplocale, retlocale);
    }
  else
    {
      prprint ("Test of XLocaleOfIM() is succeeded.\n\n");
      fprintf (icfp, "Test of XLocaleOfIM() is succeeded.\n");
      fprintf (icfp, "name of locale : %s, name of retruned locale : %s\n\n", tmplocale, retlocale);
    }
  prpok ();
}
