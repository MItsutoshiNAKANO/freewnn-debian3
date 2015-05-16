/*
 * $Id: cursor.c,v 1.2 2001/06/14 18:16:14 ura Exp $
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
#include <stdio.h>
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

#ifdef  XJUTIL
int cursor_colum = 0;
static int cursor_reverse = 0;
static int cursor_underline = 0;
static int cursor_bold = 0;
int cursor_invisible = 1;
#endif /* XJUTIL */

void
throw_col (col)
     int col;
{
  throw_cur_raw (col);
  cursor_colum = col;
}

void
h_r_on ()
{
  if (!cursor_reverse)
    {
      h_r_on_raw ();
      cursor_reverse = 1;
    }
}

void
h_r_off ()
{
  if (cursor_reverse)
    {
      h_r_off_raw ();
      cursor_reverse = 0;
    }
  if (cursor_bold)
    {
      b_s_on_raw ();
    }
}

void
u_s_on ()
{
  if (!cursor_underline)
    {
      u_s_on_raw ();
      cursor_underline = 1;
    }
}

void
u_s_off ()
{
  if (cursor_underline)
    {
      u_s_off_raw ();
      cursor_underline = 0;
    }
}

static void
b_s_on ()
{
  if (!cursor_bold)
    {
      b_s_on_raw ();
      cursor_bold = 1;
    }
  if (cursor_reverse)
    {
      h_r_on_raw ();
    }
}

static void
b_s_off ()
{
  if (cursor_bold)
    {
      b_s_off_raw ();
      cursor_bold = 0;
    }
}

void
kk_cursor_invisible ()
{
  if (cursor_invisible == 0)
    {
      cursor_invisible_raw ();
      cursor_invisible = 1;
    }
}

void
kk_cursor_normal ()
{
  if (cursor_invisible)
    {
      cursor_normal_raw ();
      cursor_invisible = 0;
    }
}

void
reset_cursor_status ()
{
  kk_cursor_normal ();
  h_r_off ();
  u_s_off ();
}

void
set_cursor_status ()
{
  if (cursor_invisible)
    {
      cursor_invisible_raw ();
    }
  else
    {
      cursor_normal_raw ();
    }
  if (cursor_reverse)
    {
      h_r_on_raw ();
    }
  if (cursor_underline)
    {
      u_s_on_raw ();
    }
  throw_cur_raw (cursor_colum);
}

void
clr_line_all ()
{
  throw_cur_raw (0);
  clr_end_screen ();
}

static int saved_cursor_rev;
static int saved_cursor_und;
void
push_hrus ()
{
  saved_cursor_rev = cursor_reverse;
  saved_cursor_und = cursor_underline;
  h_r_off ();
  u_s_off ();
}

void
pop_hrus ()
{
  if (saved_cursor_rev)
    h_r_on ();
  if (saved_cursor_und)
    u_s_on ();
}

void
set_hanten_ul (x, y)
     int x, y;
{
  if (!x)
    h_r_off ();
  if (!y)
    u_s_off ();
  if (x)
    h_r_on ();
  if (y)
    u_s_on ();
}

void
set_bold (x)
     int x;
{
  if (x)
    b_s_on ();
}

void
reset_bold (x)
     int x;
{
  if (x)
    b_s_off ();
}
