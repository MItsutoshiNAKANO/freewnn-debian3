/*
 * $Id: chkcset.c,v 1.4.2.1 1999/02/08 06:02:49 yamasita Exp $
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
 * chkcset.c v 1.0   Tue Mar 12 20:35:00 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"
#include "func.h"

/* *INDENT-OFF* */
FLAG
xsuicf (ic, mode)               /* XSet/Unset IC Focus */
    XIC ic;
    FLAG mode;
/* *INDENT-ON* */
{
  cls (prdisp);
  prstatus ("Test of XSet/UnsetICFocus()...");
  switch (focuson)
    {
    case OK:
      XUnsetICFocus (ic);
      focuson = NG;
      prprint ("The IC Focus is unset by calling XUnsetICFocus.\n\n");
      prprint ("Check your own eyes.\n\n");
      prprint ("If you want to set IC,\n");
      prprint ("you have to click your mouse ");
      prprint ("\"Focus1\" on menu window again.\n\n");
      cwcolor (ufname, pixel[cmflag][1]);
      fprintf (icfp, "...XUnsetICFocus() returned.\n\n");
      break;
    default:
      XSetICFocus (ic);
      focuson = OK;
      prprint ("The IC Focus is set by calling XSetICFocus.\n\n");
      prprint ("Check your own eyes.\n\n");
      prprint ("If you want to unset IC,\n");
      prprint ("you have to click your mouse ");
      prprint ("\"Focus1\" on menu window again.\n\n");
      cwcolor (ufname, pixel[cmflag][0]);
      fprintf (icfp, "...XSetICFocus() returned.\n\n");
      break;
    }
  prstatus ("done.");
  prprint ("Move next test.\n\n");
  prpok ();
  return (COMP);
}

/* *INDENT-OFF* */
FLAG
sfocus (ic, mode)               /* Set FOCUS window */
    XIC ic;
    FLAG mode;
/* *INDENT-ON* */

{
  Window gfocus;
  VALUABLE *focus;
  int res;

  if ((mode != DUMMY) && (focuson == OK))
    {
      cls (prdisp);
      prstatus ("Test of XSetICValues() for change \"Focus Window\"...");
      if (ufocus == window1.win)
        {
          cwcolor (ufname, pixel[cmflag][1]);
          ufocus = window2.win;
          ufname = window2.name;
          cwcolor (ufname, pixel[cmflag][0]);
        }
      else if (ufocus == window2.win)
        {
          cwcolor (ufname, pixel[cmflag][1]);
          ufocus = window3.win;
          ufname = window3.name;
          cwcolor (ufname, pixel[cmflag][0]);
        }
      else if (ufocus == window3.win)
        {
          cwcolor (ufname, pixel[cmflag][1]);
          ufocus = window1.win;
          ufname = window1.name;
          cwcolor (ufname, pixel[cmflag][0]);
        }
      XSetICValues (ic, XNFocusWindow, ufocus, NULL);
      prstatus ("done.");
      prstatus ("Test of XGetICValues() \"FocusWindow\"...");
      XGetICValues (ic, XNFocusWindow, &gfocus, NULL);
      prstatus ("done.");
      res = 0;
      focus = mkstruct (FW, HEX, ufocus, gfocus);
      res = verval (focus);
      cls (prdisp);
      prveres (res);
      if (res != NOERR)
        {
          prprint ("Test of XSetICValues() \"FocusWindow\" is failed.\n");
          prprint ("FocusWindow has not changed.\n\n");
          prprint ("I will destroy IC and continue test.\n");
          prprint ("Check log file after this test.\n\n");
          fprintf (icfp, "...Test of XSetICValues() \"FocusWindow\" is failed.\n\n");
          prpng ();
          return (DIC);
        }
      prprint ("Test of XSetICValues() \"FocusWindow\" is succeeded.\n");
      prprint ("FocusWindow has changed.\n\n");
      prprint ("Check your own eyes and move next test.\n\n");
      fprintf (icfp, "...Test of XSetICValues() \"FocusWindow\" is succeeded.\n\n");
      prpok ();
      return (COMP);
    }
  else
    {
      cls (prdisp);
      prprint ("You can't check this test.\n\n");
      prprint ("Retry after \"Focus1\" test.\n\n");
      fprintf (icfp, "...Retry after \"Focus1\" test.\n\n");
    }
  return (COMP);
}


/* *INDENT-OFF* */
FLAG
sresource (ic, mode)            /* Set RESOURCE name/class */
    XIC ic;
    FLAG mode;
/* *INDENT-ON* */

{
  int sum;
  int cnt, res;
  RESOURCE gres;
  VALUABLE *resource[ARRAY_MAX];

  if (mode == ONSP)
    {
      prerrfunc ();
      return (COMP);
    }
  else
    {
      cls (prdisp);
      prstatus ("Test of XSetICValues() for change \"Resource Name\",\"Resource Class\"...");
      if (!strcmp (ures.name, "xim"))
        ures.name = "xima";
      else if (!strcmp (ures.name, "xima"))
        ures.name = "ximb";
      else if (!strcmp (ures.name, "ximb"))
        ures.name = "xim";
      if (!strcmp (ures.class, "Xim"))
        ures.class = "Xima";
      else if (!strcmp (ures.class, "Xima"))
        ures.class = "Ximb";
      else if (!strcmp (ures.class, "Ximb"))
        ures.class = "Xim";
      XSetICValues (ic, XNResourceName, ures.name, XNResourceClass, ures.class, NULL);
      prstatus ("done.");
      prstatus ("Test of XGetICValues() \"resource name/class\"...");
      XGetICValues (ic, XNResourceName, &gres.name, XNResourceClass, &gres.class, NULL);
      prstatus ("done.");
      sum = 0;
      resource[0] = mkstruct (RN, STR, ures.name, gres.name);
      resource[1] = mkstruct (RC, STR, ures.class, gres.class);
      for (cnt = 0; cnt < CHKRES; cnt++)
        {
          res = verval (resource[cnt]);
          sum += res;
        }
      cls (prdisp);
      prveres (sum);
      if (sum != NOERR)
        {
          prprint ("Test of XSetICValues() \"ResourceName/Class\" is failed.\n");
          prprint ("ResourceName/ResourceClass has not changed.\n\n");
          prprint ("I will destroy IC and continue test.\n");
          prprint ("Check log file after this test.\n\n");
          fprintf (icfp, "...Test of XSetICValues() \"ResourceName/Class\" is failed.\n\n");
          prpng ();
          return (DIC);
        }
      prprint ("Test of XSetICValues() \"ResourceName/Class\" is succeeded.\n");
      prprint ("ResourceName/ResourceClass has changed.\n\n");
      prprint ("Check your own eyes and move next test\n\n");
      fprintf (icfp, "...Test of XSetICValues() \"ResourceName/Class\" is succeeded.\n\n");
      prpok ();
      return (COMP);
    }
}
