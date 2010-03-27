/*
 * $Id: chkim.c,v 1.2 2001/06/14 18:16:11 ura Exp $
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
 * chkim.c v 1.0   Tue Mar  5 16:18:15 JST 1991
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

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Xresource.h>

/* *INDENT-OFF* */
XIM
openim (lfdir, flag)            /* check of XOpenIM */
     char *lfdir;
     FLAG *flag;
/* *INDENT-ON* */
{
  XIM im;
  char *home;
  char logfile[MAX_BUF];
  char dbsrc[MAX_BUF];
  XrmDatabase rdb;
  XIMStyles *ximstyle;
  XIMStyle *style;
#ifdef XML
  char *languages;
#endif /* XML */
  int cnt;

  char *getenv ();

  prmode ("X11R5 Input Method Test Program : <<IM Mode>>");
  prstatus ("Creating log file for im...");     /* open log file for IM test */
  sprintf (logfile, "%s/tim.log", lfdir);
  imfp = fopen (logfile, "w");
  prstatus ("done.");
  if (imfp == NULL)
    {
      cls (prdisp);
      prerrld (logfile);
    }

  prstatus ("Creating resource database...");   /* create resource database */
  home = getenv ("HOME");
  sprintf (dbsrc, "%s/.Xdefaults", home);
  rdb = XrmGetFileDatabase (dbsrc);
  prstatus ("done.");

  prstatus ("Creating input method...");        /* open IM */
  im = XOpenIM (dpy, rdb, "xim", "XIM");
  prstatus ("done.");
  cls (prdisp);
  if (im == NULL)
    {                           /* for XOpenIM error */
      prprint ("Test of XOpenIM() is failed.\n\n");
      prprint ("Check environmental valuable \"XNLSPATH\"\n\n");
      prprint ("and make sure to run \"xwnmo\".\n\n");
      prprint ("This program will shutdown soon.\n\n");
      fprintf (imfp, "Test of XOpenIM() is failed.\n");
      fprintf (imfp, "\"NULL\" returned.\n\n");
      *flag = OPIMERR;
      return (im);
    }
  prprint ("Test of XOpenIM() is succeeded.\n");        /* for XOpenIM success */
  prprint ("Move next test.\n\n");
  fprintf (imfp, "Test of XOpenIM() is succeeded.\n");
  fprintf (imfp, "im : 0x%X\n\n", im);

#ifdef XML
  languages = 0;                /* initialization of valuables */
#endif /* XML */
  prstatus ("Test of XGetIMValues()...");       /* test of XGetIMValues */
  XGetIMValues (im, XNQueryInputStyle, &ximstyle,
#ifdef XML
                XNQueryLanguage, &languages,
#endif /* XML */
                NULL);
  prstatus ("done.");
  if (ximstyle->count_styles == 0)
    {                           /* for XGetIMValues error */
      prprint ("Test of XGetIMValues() is failed.\n");
      prprint ("This program will shutdown soon.\n\n");
      fprintf (imfp, "Test of XGetIMValues() is failed. ");
      fprintf (imfp, "ximstyle returned \"NULL\".\n\n");
      *flag = GIMERR;
      XFree (ximstyle);
      return (NULL);
    }
  prprint ("Test of XGetIMValues() is succeeded.\n");
  prprint ("Move next test.\n\n");
  /* for XGetIMValues success */
  fprintf (imfp, "Test of XGetIMValues() is succeeded.\n");
  for (cnt = 0, style = ximstyle->supported_styles; cnt < ximstyle->count_styles; cnt++, style++)
    fprintf (imfp, "input style : 0x%X\n", *style);     /* write values to log file */
#ifdef XML
  fprintf (imfp, "You can use languages in this IM : %s\n\n", languages);
#endif /* XML */
  XFree (ximstyle);
  return (im);
}

void
closeim (im)                    /* CLOSE IM */
     XIM im;
{
  XCloseIM (im);
}

void
fclim ()                        /* log File CLose for IM */
{
  fclose (imfp);
}
