/*
 *  $Id: redraw.c $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000
 *
 * Maintainer:  FreeWnn Project   <freewnn@tomo.gr.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* REDRAW HENKAN LINE */

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "buffer.h"

int
reset_line ()
{
#if defined(uniosu)
  putchar (Ctrl ('O'));         /* おまじない。括弧の中はゼロでなくオー */
#endif /* defined(uniosu) */
  if (empty_modep () == 0)
    {
      push_cursor ();
      set_scroll_region (0, crow - 1);
      kk_restore_cursor ();
      call_redraw_line (c_b->t_c_p, 1);
      pop_cursor ();
    }
  return (0);
}

int
redraw_line ()
{
  throw_col (0);
  /* 面倒でももう一回スクロール領域を切ろう。 */
  disp_mode ();
  if (!empty_modep ())
    {
      if (not_redraw == 0)
        {
          clr_line_all ();
          (*t_print_l_func) ();
        }
      else
        {
          throw_c (0);
        }
    }
  else
    {
      kk_restore_cursor ();
    }
  flush ();
  return (0);
}
