/*
 * $Id: funcv.c,v 1.2 2001/06/14 18:16:11 ura Exp $
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
 * funcv.c v 1.0   Fri Mar  8 14:51:03 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"

VALUABLE *
mkstruct (valname, mode, setval, getval)        /* MaKe STRUCT valuable */
     char *valname;
     FLAG mode;
#ifdef SYSV
     caddr_t *setval;
     caddr_t *getval;
#else
     void *setval;
     void *getval;
#endif
{
  VALUABLE *p;

  p = (VALUABLE *) malloc (sizeof (VALUABLE));
  p->vname = valname;
  p->mode = mode;
  p->sval = setval;
  p->gval = getval;
  return (p);
}

/* *INDENT-OFF* */
FLAG
verval (va_st)                  /* VERify VALuable */
     VALUABLE *va_st;
/* *INDENT-ON* */

{
  switch (va_st->mode)
    {
    case STR:
      fprintf (icfp, "value %-35s: set -> %-10s, get -> %-10s", va_st->vname, va_st->sval, va_st->gval);
      if (!strcmp (va_st->sval, va_st->gval))
        {
          fprintf (icfp, "\n");
          return (OK);
        }
      fprintf (icfp, "\t...Failed.\n");
      return (NG);
      break;
    case HEX:
      fprintf (icfp, "value %-35s: set -> 0x%-8X, get -> 0x%-8X", va_st->vname, va_st->sval, va_st->gval);
      break;
    case DEC:
      fprintf (icfp, "value %-35s: set -> %-10d, get -> %-10d", va_st->vname, va_st->sval, va_st->gval);
      break;
    }
  if ((strcmp (va_st->vname, PNW)) && (strcmp (va_st->vname, PNH)) && (strcmp (va_st->vname, SNW)) && (strcmp (va_st->vname, SNH)))
    {
      if (va_st->sval != va_st->gval)
        {
          fprintf (icfp, "\t...Failed.\n");
          return (NG);
        }
    }
  fprintf (icfp, "\n");
  return (OK);
}
