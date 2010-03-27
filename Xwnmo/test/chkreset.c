/*
 * $Id: chkreset.c,v 1.2 2001/06/14 18:16:11 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
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
 * ximtest.c v 1.0   Mon Mar 18 15:05:15 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>
#include "exvalue.h"
#include "func.h"

/* *INDENT-OFF* */
FLAG
preset (ic, mode)               /* Preedit area RESET */
    XIC ic;
    FLAG mode;
/* *INDENT-ON* */
{
  if (mode == ONSP)
    {
      prerrfunc ();
      return (COMP);
    }
  else
    {
      cls (prdisp);
      if (strflag == MB)
        {
          prstatus ("Test of XmbResetIC()...");
          XmbResetIC (ic);
        }
      else if (strflag == WC)
        {
          prstatus ("Test of XwcResetIC()...");
          XwcResetIC (ic);
        }
      prstatus ("done.");
      prprint ("Check your own eyes if preedit area on the test window is clear.\n\n");
      prprint ("And re-input string.\n\n");
      if (strflag == MB)
        {
          fprintf (icfp, "...XmbResetIC returned.\n\n");
        }
      else if (strflag == WC)
        {
          fprintf (icfp, "...XwcResetIC returned.\n\n");
        }
      return (COMP);
    }
}
