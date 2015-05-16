/*
 * $Id: exvalue.h,v 1.3.2.1 1999/02/08 06:02:55 yamasita Exp $
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
 * exvalue.h v 1.0   Tue Mar  5 12:10:42 JST 1991
 */

/*
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <curses.h>

#include "define.h"
#include "typedef.h"

/* valuable of main window */
extern WINDOW *prdisp, *modisp, *stdisp;

/* valuable of X resource */
extern Display *dpy;
extern int scr;
extern Window rwin;
extern TESTWIN window1, window2, window3;
extern Window mroot, mwin[];
extern XFontSet fontset[];
extern char bit_bits[ARRAY_MAX][BITS_MAX];
extern Pixmap pix[];
extern u_long pixel[ARRAY_MAX][PIX_MAX];
extern Colormap cmap[];
extern Colormap dcmap;
extern GC gc;
extern CALLBACK upec, ustc;
extern Cursor cursor[];

/* valuable of pointer to logfile */
extern FILE *imfp;              /* log file for im test */
extern FILE *icfp;              /* log file for ic test */

/* valuable to use input context */
extern XIMStyle uistyle;        /* Using Input STYLE */
extern Window uclient;          /* Using CLIENT window */
extern Window ufocus;           /* Using FOCUS window */
extern RESOURCE ures;           /* Using RESource name/class */
extern PREEDIT upea;            /* Using PreEdit Area */
extern STATUS usta;             /* Using STatus Area */
extern FLAG cmflag;             /* ColorMap FLAG */
extern FLAG focuson;            /* flag for FOCUS ON/off */
extern Window ufname;           /* Using Focus window NAME */

extern SETICMF ftbl[];
extern Window strmode;          /* STRing MODE(MB or WC) */
extern FLAG strflag;            /* STRing FLAG(MB or WC) */
