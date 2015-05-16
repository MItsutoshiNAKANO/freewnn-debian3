/*
 * $Id: ximtest.c,v 1.4.2.1 1999/02/08 06:02:58 yamasita Exp $
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
 * ximtest.c v 1.0   Fri Mar  1 13:04:41 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <curses.h>

#include "typedef.h"
#include "define.h"
#include "func.h"

/* valuable of main window */
WINDOW *prdisp, *modisp, *stdisp;

/* valuable of X window resouce */
Display *dpy;
int scr;
Window rwin;
TESTWIN window1, window2, window3;
Window mroot, mwin[MENU_MAX];
XFontSet fontset[ARRAY_MAX];
GC gc;
Colormap cmap[ARRAY_MAX];
Colormap dcmap;

u_long pixel[ARRAY_MAX][PIX_MAX];
/* u_long               pixel[ARRAY_MAX][PIX_MAX] = {
    {0L, 1L, 2L, 3L, 4L, 5L},
    {0L, 1L, 2L, 3L, 4L, 5L}
    }; */

char bit_bits[ARRAY_MAX][BITS_MAX] = {
  {0xff, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
   0x21, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00},
  {0x21, 0x00, 0x02, 0x02, 0x04, 0x01, 0x88, 0x00, 0x50, 0x00,
   0x21, 0x00, 0x50, 0x00, 0x88, 0x00, 0x04, 0x01, 0x02, 0x02}
};
Pixmap pix[ARRAY_MAX];
Cursor cursor[ARRAY_MAX];

/* valuable to use input context */
XIMStyle uistyle;               /* Using Input STYLE */
Window uclient;                 /* Using CLIENT window */
Window ufocus;                  /* Using FOCUS window */
RESOURCE ures;                  /* Using RESource name/class */
PREEDIT upea;                   /* Using PreEdit Area */
STATUS usta;                    /* Using STatus Area */
CALLBACK upec;                  /* Using PreEdit Callback */
CALLBACK ustc;                  /* Using STatus Callback */
FLAG cmflag = 0;                /* ColorMap FLAG */
FLAG focuson = NG;              /* flag for FOCUS ON/off */
Window ufname;                  /* Using Focus window NAME */

SETICMF ftbl[] = {
  {&mwin[0], "Focus1", xsuicf, SUICF},
  {&mwin[1], "Focus2", sfocus, FOCUS},
  {&mwin[2], "Resource", sresource, RESNC},
  {&mwin[3], "Area", sarea, AREA},
  {&mwin[4], "Needed", sneeded, NEEDED},
  {&mwin[5], "Spot", sspot, SPOT},
  {&mwin[6], "Colormap", scmap, COLORMAP},
  {&mwin[7], "Fg/Bg", sground, GROUND},
  {&mwin[8], "Pixmap", spixmap, PIXMAP},
  {&mwin[9], "FontSet", sfontset, FONTSET},
  {&mwin[10], "Line", slinespc, LINESPC},
  {&mwin[11], "Cursor", scursor, CURSOR},
  {&mwin[12], "Reset", preset, RESET},
  {&mwin[13], "Chmod", NULL, CHMOD},
  {NULL, NULL, NULL, 0}
};

FLAG strflag;                   /* STRing FLAG (MB or WC) */
Window strmode;                 /* STRing mode (MB or WC) */
/* valuable of pointer to logfile */
FILE *imfp;                     /* log file for im test */
FILE *icfp;                     /* log file for ic test */

void
main (argc, argv)               /* MAIN program */
     int argc;
     char **argv;
{
  FLAG mode = 0;                /* test mode */
  FLAG res = 0;                 /* result of test */
  FLAG flag;                    /* error flag for open im & create ic */
  char *langname;               /* language name for use */
  char *locale, tmplocale[32];  /* locale name for use */
  char *ldenv;                  /* Log file Directory written in ENVironment valuables */
  char lduse[MAX_BUF];          /* Log file Directory for use actually */
  XIM im;                       /* input method to use */
  XIC ic;                       /* input context to use */

  char *getenv ();

  if (argc < 2)
    {
      printf ("\nError : No argument.\n");
      printf ("Usage : command (log-file-directory)\n\nBye.\n");
      exit (ENV_ERR);
    }
  else if (argc == 2)
    {
      ldenv = getenv ("XIMLOGDIR");
      if (ldenv == NULL)
        {
          printf ("Error : No set environment valuable.\n");
          printf ("Please write \"setenv XIMLOGDIR (log-file-directory)\"\n\nBye.\n");
          exit (ENV_ERR);
        }
      else
        {
          sprintf (lduse, "%s/%s", ldenv, argv[1]);
          mkdir (lduse, 0775);  /* make directory for log file */
        }
    }
  else
    {
      printf ("\nError : Too many arguments.\n");
      printf ("Usage : command (log-file-directory)\n\nBye.");
      exit (ENV_ERR);
    }

  langname = getenv ("LANG");   /* set environmental valuable */
  if (langname == NULL)
    {
      printf ("I don't know language to use\n");
      exit (ENV_ERR);
    }
  if ((locale = setlocale (LC_ALL, langname)) == NULL)
    {                           /* set locale */
      printf ("I cound not set locale\n");
      exit (ENV_ERR);
    }
  strcpy (tmplocale, locale);
/*
    _Xsetlocale(LC_ALL, locale);
*/
  XSetLocaleModifiers ("");

  initmwin ();                  /* initialize of main window */
  prtitle ();                   /* print title */
  createxres ();                /* create X resources */

  im = openim (lduse, &flag);   /* open IM */
  if (im == NULL)
    {
      if (flag == OPIMERR)
        fcimtoex (imfp);
      else if (flag == GIMERR)
        climtoex (imfp);
    }
  for (;;)
    {
      if (res != DIC)
        {
          mode = psmenu ();
        }
      if (mode == ENDAT)
        {
          if (ic != NULL)
            {
              dtictoex (ic, im);
            }
          climtoex (im);
        }
      ic = createic (im, mode, lduse, &flag);   /* create IC on each mode */
      if (ic == NULL)
        {
          if (flag == CRICERR)
            fcictoex (ic, im);
          else if (flag == GICERR)
            dtictoex (ic, im);
        }
      chkutil (im, ic, tmplocale);      /* check utility functions */
      res = keyev (ic, mode);   /* transaction key event */
      switch (res)
        {
        case COMP:              /* test completed on some mode */
          break;
        case DIC:               /* destroy IC and continue test */
          dticonly (ic);
          break;
        case MENU:              /* return main menu on main window */
          dtictofc (ic);
          break;
        }
    }
}
