/*
 * $Id: final.c,v 1.1.2.1 1999/02/08 06:02:55 yamasita Exp $
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
 * final.c v 1.0   Fri Mar 15 14:01:00 JST 1991
 */

/*
 *      Author: Shoji kamada     OMRON Corporation
 *                               kamada@ari.ncl.omron.co.jp
 *
 *              Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"
#include "func.h"

void
dtictoex (ic, im)               /* DesTroy IC TO EXit */
     XIM im;
     XIC ic;
{
  dticonly (ic);
  fcictoex (im);
}

void
dticonly (ic)                   /* DesTroy IC ONLY */
     XIC ic;
{
  destic (ic);
}

void
dtictofc (ic)                   /* DesTroy IC TO File Close of ic */
     XIC ic;
{
  dticonly (ic);
  fclic ();
}

void
fcictoex (im)                   /* log File Close of IC TO EXit */
     XIM im;
{
  fclic ();
  climtoex (im);
}

void
climtoex (im)                   /* CLose IM TO EXit */
     XIM im;
{
  closeim (im);
  fcimtoex ();
}

void
fcimtoex ()                     /* log File Close IM TO EXit */
{
  fclim ();
  freexres ();
  freemwin ();
}
