/*
 * $Id: typedef.h,v 1.1.2.1 1999/02/08 06:02:57 yamasita Exp $
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
 * typedef.h v 1.0   Fri Mar 22 11:26:47 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>
#include "Xi18nint.h"

typedef unsigned long FLAG;

typedef struct _testwin
{                               /* Test window */
  Window win;                   /* test WINdow */
  Window name;                  /* Name window of test window */
}
TESTWIN;

typedef struct _resource
{                               /* RESOURCE name/class structure */
  char *name;                   /* resource NAME */
  char *class;                  /* resource CLASS */
}
RESOURCE;

typedef struct _preedit
{                               /* PREEDIT attribution structure */
  XRectangle area;              /* preedit-AREA */
  XRectangle needed;            /* preedit-areaNEEDED */
  XPoint spot;                  /* preedit-SPOTlocation */
  Colormap cmap;                /* preedit-ColorMAP */
  u_long fg;                    /* preedit-ForeGround */
  u_long bg;                    /* preedit-BackGround */
  Pixmap bgpm;                  /* preedit-BG/ PixMap */
  XFontSet fset;                /* preedit-FontSET */
  int lspc;                     /* preedit-LineSPaCe */
  Cursor cursor;                /* preedit-CURSOR */
}
PREEDIT;

typedef struct _status
{                               /* STATUS attribution structure */
  XRectangle area;              /* status-AREA */
  XRectangle needed;            /* status-areaNEEDED */
  Colormap cmap;                /* status-ColorMAP */
  u_long fg;                    /* status-ForeGround */
  u_long bg;                    /* status-BackGround */
  Pixmap bgpm;                  /* status-BG/ PixMap */
  XFontSet fset;                /* status-FontSET */
  Cursor cursor;                /* status-CURSOR */
}
STATUS;

typedef struct _valuable
{                               /* verify VALUABLE table structure */
  char *vname;                  /* Valuable NAME */
  FLAG mode;                    /* decimal or hex or string */
#ifdef SYSV
  caddr_t sval;                 /* Set VALuable */
  caddr_t gval;                 /* Get VALuable */
#else
  void *sval;                   /* Set VALuable */
  void *gval;                   /* Get VALuable */
#endif
}
VALUABLE;

typedef struct _cricmf
{                               /* CReate IC Mode Function structure */
  FLAG mode;                    /* on or over or off or root */
    XIC (*fmust) ();            /* Function name for create ic (MUST)  */
    XIC (*fmay) ();             /* Function name for create ic (MAY) */
}
CRICMF;

typedef struct _seticmf
{                               /* SET IC Mode Function structure */
  Window *mwin;                 /* Menu Window id */
  char *mname;                  /* Menu NAME */
    FLAG (*func) ();            /* FUNCtion name for set */
  FLAG flag;                    /* error FLAG */
}
SETICMF;

typedef struct _callback
{                               /* CALLBACK structure */
  XIMCallback start;            /* START callback */
  XIMCallback done;             /* DONE callback */
  XIMCallback draw;             /* DRAW callback */
  XIMCallback caret;            /* CARET callback */
}
CALLBACK;

typedef union _string
{                               /* STRING union */
  char *multi_byte;
  wchar_t *wide_char;
}
STRING;
