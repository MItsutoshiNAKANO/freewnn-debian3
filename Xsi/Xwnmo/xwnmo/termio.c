/*
 * $Id: termio.c,v 1.2.2.1 1999/02/08 08:08:56 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright 1991 by Massachusetts Institute of Technology
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
/*      Version 4.0
 */
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */

#ifndef XJUTIL
#define cur_text        (cur_p->cur_xl)
#endif /* XJUTIL */

void
clr_end_screen ()
{
  JWMline_clear (cur_text->currentcol);
}

void
throw_cur_raw (col)
     int col;
{
  JWcursor_move (col);
}

void
h_r_on_raw ()
{
  cur_text->r_flag = 1;
}

void
h_r_off_raw ()
{
  cur_text->r_flag = 0;
}

void
u_s_on_raw ()
{
  cur_text->u_line_flag = 1;
}

void
u_s_off_raw ()
{
  cur_text->u_line_flag = 0;
}

void
b_s_on_raw ()
{
  cur_text->b_flag = 1;
}

void
b_s_off_raw ()
{
  cur_text->b_flag = 0;
}

void
ring_bell ()
{
  XBell (dpy, 0);
}

void
cursor_invisible_raw ()
{
  JWcursor_invisible ();
}

void
cursor_normal_raw ()
{
  cur_text->u_line_flag = cur_text->r_flag = 0;
  JWcursor_visible ();
}
