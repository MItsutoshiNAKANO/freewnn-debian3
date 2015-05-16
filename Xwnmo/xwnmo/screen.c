/*
 * $Id: screen.c,v 1.2 2001/06/14 18:16:17 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright 1991, 1992 by Massachusetts Institute of Technology
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
 *      '99/04/19       TAOKA Satoshi - 田岡 智志<taoka@infonets.hiroshima-u.ac.jp>
 *              index() の宣言をコメントアウト。
 *
 * Last modified date: 19,Apr.1999
 *
 * Code:
 *
 */
/*      Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "config.h"
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
#include "rk_spclval.h"
#include "rk_fundecl.h"


static int col = 0; /** cursor position from startichi*/
static int oldmaxcol = 0; /** redraw sita tokini doko made mae ni kaita data ga nokotteiruka wo simesu */

#define CHAR_BYTE 0

#define w_putchar_move(x) {col += w_putchar(x);}
#define putchar_move(x) {col += 1; putchar_norm(x);}
#define putchar_move1(x) {col += 1; put_char1(x);}

#define set_screen_reverse(X,Y) \
    {set_hanten_ul(c_b->hanten & X,c_b->hanten & Y);}

#define reset_screen_reverse(X,Y) \
    {flushw_buf(); if(c_b->hanten & X)h_r_off();if(c_b->hanten & Y)u_s_off();}

#define set_screen_bold(X) \
    {flushw_buf(); set_bold(c_b->hanten & X);}

#define reset_screen_bold(X) \
    {flushw_buf(); reset_bold(c_b->hanten & X);}

static char rk_modes[80];

void
throw (x)
     int x;
{
  throw_col ((col = x));
}

int
char_len (x)
     w_char x;
{
  return ((*char_len_func) (x));
}

/*return the position of charcter cp, when column 0 is start_point */
int
cur_ichi (cp, start_point)
     register int cp;
     int start_point;
{
  register int k;
  register int end_of_line = maxlength;
  register int len = 0;

  if (cp < start_point)
    return (-1);
  if (cp > c_b->maxlen)
    cp = c_b->maxlen;

  for (k = start_point; k < cp; k++)
    {
      len += char_len (c_b->buffer[k]);
      if (len > end_of_line - CHAR_BYTE)
        {
          len = end_of_line;
          end_of_line += maxlength;
        }
    }
  return (len);
}

void
t_print_line (st, end, clr_l)
     int st, end, clr_l;
{
  register int k;
  register int col1;
  register int end_of_line = maxlength;

  int mst = MIN_VAL (c_b->t_m_start, c_b->t_c_p);
  int mend = MAX_VAL (c_b->t_m_start, c_b->t_c_p);

  int bst = c_b->t_b_st;
  int bend = c_b->t_b_end;
  int bold = 0;

  if (end > c_b->maxlen)
    end = c_b->maxlen;
  col1 = cur_ichi (st, 0);
  if (col1 == -1)
    {
      col1 = 0;
      k = st = 0;
    }
  else
    {
      k = st;
    }

  for (; end_of_line <= col1; end_of_line += maxlength);

  throw (col1);
  if (mst >= 0)
    {
      if (st < mst)
        {
          set_screen_reverse (0x01, 0x02);
          for (k = st; k < mst; k++)
            {
              if (bold == 0 && k >= bst && k < bend)
                {
                  set_screen_bold (0x40);
                  bold = 1;
                }
              else if (bold && (k < bst || k >= bend))
                {
                  reset_screen_bold (0x40);
                  set_screen_reverse (0x01, 0x02);
                  bold = 0;
                }
              if (k >= end)
                {
                  reset_screen_reverse (0x01, 0x02);
                  reset_screen_bold (0x40);
                  bold = 0;
                  goto normal_out;
                }
              w_putchar_move (c_b->buffer[k]);
              if (col > end_of_line - CHAR_BYTE)
                {
                  flushw_buf ();
                  if (col < end_of_line)
                    {
                      reset_screen_reverse (0x01, 0x02);
                      reset_screen_bold (0x40);
                      bold = 0;
                      putchar_move (' ');
                      set_screen_reverse (0x01, 0x02);
                      if (k >= bst && k < bend)
                        {
                          set_screen_bold (0x40);
                          bold = 1;
                        }
                    }
                  throw (end_of_line);
                  end_of_line += maxlength;
                }
            }
          reset_screen_reverse (0x01, 0x02);
          reset_screen_bold (0x40);
          bold = 0;
        }
      if (k < mend)
        {
          set_screen_reverse (0x04, 0x08);
          for (; k < mend; k++)
            {
              if (bold == 0 && k >= bst && k < bend)
                {
                  set_screen_bold (0x40);
                  bold = 1;
                }
              else if (bold && (k < bst || k >= bend))
                {
                  reset_screen_bold (0x40);
                  set_screen_reverse (0x04, 0x08);
                  bold = 0;
                }
              if (k >= end)
                {
                  reset_screen_reverse (0x04, 0x08);
                  reset_screen_bold (0x40);
                  bold = 0;
                  goto normal_out;
                }
              w_putchar_move (c_b->buffer[k]);
              if (col > end_of_line - CHAR_BYTE)
                {
                  flushw_buf ();
                  if (col < end_of_line)
                    {
                      reset_screen_reverse (0x04, 0x08);
                      reset_screen_bold (0x40);
                      bold = 0;
                      putchar_move (' ');
                      set_screen_reverse (0x04, 0x08);
                      if (k >= bst && k < bend)
                        {
                          set_screen_bold (0x40);
                          bold = 1;
                        }
                    }
                  throw (end_of_line);
                  end_of_line += maxlength;
                }
            }
          reset_screen_reverse (0x04, 0x08);
          reset_screen_bold (0x40);
          bold = 0;
        }
    }
  if (k < c_b->maxlen && k < end)
    {
      set_screen_reverse (0x10, 0x20);
      for (; k < c_b->maxlen; k++)
        {
          if (bold == 0 && k >= bst && k < bend)
            {
              set_screen_bold (0x40);
              bold = 1;
            }
          else if (bold && (k < bst || k >= bend))
            {
              reset_screen_bold (0x40);
              set_screen_reverse (0x10, 0x20);
              bold = 0;
            }
          if (k >= end)
            {
              reset_screen_reverse (0x10, 0x20);
              reset_screen_bold (0x40);
              bold = 0;
              goto normal_out;
            }
          w_putchar_move (c_b->buffer[k]);
          if (col > end_of_line - CHAR_BYTE)
            {
              flushw_buf ();
              if (col < end_of_line)
                {
                  reset_screen_reverse (0x10, 0x20);
                  reset_screen_bold (0x40);
                  bold = 0;
                  putchar_move (' ');
                  set_screen_reverse (0x10, 0x20);
                  if (k >= bst && k < bend)
                    {
                      set_screen_bold (0x40);
                      bold = 1;
                    }
                }
              throw (end_of_line);
              end_of_line += maxlength;
            }
        }
      reset_screen_reverse (0x10, 0x20);
      reset_screen_bold (0x40);
      bold = 0;
    }

normal_out:
  if (clr_l == 1)
    {
      clr_end_screen ();
      oldmaxcol = col;
    }
  else if (clr_l == 2)
    {
      clr_end_screen ();
      oldmaxcol = col;
    }
  else
    {
      oldmaxcol = MAX_VAL (oldmaxcol, col);
    }
  return;
}

void
t_redraw_one_line ()
{
  t_print_line (0, c_b->maxlen, 1);
}

void
init_screen ()
{
  if (c_b->maxlen)
    t_print_l ();
}

void
t_redraw_move (x, start, end, clr_l)
     int x, start, end, clr_l;
{
  (*t_redraw_move_func) (x, start, end, clr_l);
}

void
t_move (x)
     int x;
{
  int old_cp = c_b->t_c_p;

  if (((c_b->hanten >> 2) & 0x3) != ((c_b->hanten >> 4) & 0x3))
    {
      t_redraw_move_normal (x, MIN_VAL (old_cp, x), MAX_VAL (old_cp, x), 0);
      return;
    }
  if (x >= c_b->maxlen)
    x = c_b->maxlen;
  c_b->t_c_p = x;
  throw (cur_ichi (c_b->t_c_p, 0));
  return;
}

void
t_print_l ()
{
  (*t_print_l_func) ();
}

char *
get_rk_modes ()
{
  char *p;
/*    extern char *index();*/

  strcpy (rk_modes, (NULL == (p = romkan_dispmode ())? "[   ]" : p));
  if ((p = (char *) index (rk_modes, ':')) != NULL && *(p + 1))
    {
      set_cur_env (*(++p));
      *p = '\0';
    }
  return (rk_modes);
}

void
disp_mode ()
{
#ifndef XJUTIL
  if (cur_p == cur_x->root_pointer->ximclient)
    visible_root ();
#endif
  jw_disp_mode (get_rk_modes ());
}

/* cursor status is saved before call it */
void
display_henkan_off_mode ()
{
  char *p;

#ifndef XJUTIL
  if (cur_p == cur_x->root_pointer->ximclient)
    invisible_root ();
#endif
  strcpy (rk_modes, (NULL == (p = romkan_offmode ())? "[---]" : p));
  jw_disp_mode (rk_modes);
}
