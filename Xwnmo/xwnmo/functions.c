/*
 * $Id: functions.c,v 1.2 2001/06/14 18:16:15 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
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

int
bytcntfn (c)
     register unsigned char *c;
{
  return (get_cswidth_by_char (*c));
}

void
conv_ltr_to_ieuc (ltr)
     register unsigned int *ltr;
{
  register int i, j;
  register unsigned int l;
  unsigned char tmp[4], c;
  w_char ret;

/*
    while ((l = *ltr) != EOLTTR) {
*/
  l = *ltr;
  while (l != EOLTTR)
    {
      if (!isSPCL (l))
        {
          for (i = 2, j = 0; i >= 0; i--)
            {
              if ((c = (l >> ((i) * 8)) & 0xff))
                tmp[j++] = c;
            }
          if (j && (eeuc_to_ieuc (&ret, tmp, j) > 0))
            {
              *ltr = (ret | (l & 0xff000000));
            }
        }
      ltr++;
      l = *ltr;
    }

}

/*
int letterpickfn(c, cnt, end_c)
register unsigned char *c;
int *cnt;
unsigned char end_c;
{
    w_char letter = (w_char)0;
    register int len, i;

    len = bytcntfn(c);
    if (end_c) {
        for (i = 1; i < len; i++) {
            if (*(c + i) == end_c) {
                *cnt = i;
                return((w_char)*c && 0xff);
            }
        }
    }
    eeuc_to_ieuc(&letter, c, len);
    *cnt = len;
    return(letter);
}
*/

int
t_redraw_move_normal (x, start, end, clr_l)
     int x, start, end, clr_l;
{
  c_b->t_c_p = x;
  t_print_line (start, end, clr_l);
  throw (cur_ichi (c_b->t_c_p, 0));
  return (0);
}

int
t_print_l_normal ()
{
  clr_line_all ();
  t_redraw_one_line ();
  throw (cur_ichi (c_b->t_c_p, 0));
  return (0);
}

int
call_t_redraw_move_normal (x, start, end, clt_l, add)
     int x, start, end, clt_l, add;
{
  t_redraw_move (x, start, end, clt_l);
  return (0);
}

int
call_t_redraw_move (x, start, end, clt_l, add)
     int x, start, end, clt_l, add;
{
  (*call_t_redraw_move_func) (x, start, end, clt_l, add);
  return (0);
}

int
call_t_redraw_move_1_normal (x, start, end, clt_l, add1, add2, mode)
     int x, start, end, clt_l, add1, add2, mode;
{
  t_move (x + add2);
  return (0);
}

int
call_t_redraw_move_1 (x, start, end, clt_l, add1, add2, mode)
     int x, start, end, clt_l, add1, add2, mode;
{
  (*call_t_redraw_move_1_func) (x, start, end, clt_l, add1, add2, mode);
  return (0);
}

int
call_t_redraw_move_2_normal (x, start1, start2, end1, end2, clt_l, add)
     int x, start1, start2, end1, end2, clt_l, add;
{
  t_move (x);
  return (0);
}

int
call_t_redraw_move_2 (x, start1, start2, end1, end2, clt_l, add)
     int x, start1, start2, end1, end2, clt_l, add;
{
  (*call_t_redraw_move_2_func) (x, start1, start2, end1, end2, clt_l, add);
  return (0);
}

int
call_t_print_l_normal (x, add)
     int x, add;
{
  t_print_l ();
  return (0);
}

int
call_t_print_l (x, add)
     int x, add;
{
  (*call_t_print_l_func) (x, add);
  return (0);
}

int
c_top_normal ()
{
  if (c_b->t_m_start < 0)
    {
      t_move (0);
    }
  else
    {
      t_move (c_b->t_m_start);
    }
  return (0);
}


int
c_end_normal ()
{
  t_move (c_b->maxlen);
  return (0);
}

int
char_q_len_normal (x)
     w_char x;
{
  return ((hankaku (x)) ? 1 : 2);
}

int
char_len_normal (x)
     w_char x;
{
  return (char_q_len_normal (x));
}

#ifndef XJUTIL
int
call_jl_yomi_len ()
{
  return (jl_yomi_len (bun_data_, cur_bnst_, dai_end (bun_data_, cur_bnst_)));
}
#endif /* !XJUTIL */

#ifdef  CHINESE
int
t_redraw_move_yincod (x, start, end, clr_l)
     int x, start, end, clr_l;
{
  c_b->t_c_p = x;
  t_print_line (start, end, clr_l);
  if (c_b->t_c_p == c_b->maxlen)
    kk_cursor_normal ();
  else
    kk_cursor_invisible ();
  throw (cur_ichi (c_b->t_c_p, 0));
  return (0);
}

int
t_print_l_yincod ()
{
  clr_line_all ();
  t_redraw_move_yincod (c_b->t_c_p, 0, c_b->maxlen, 1);
  return (0);
}

int
call_t_redraw_move_yincod (x, start, end, clt_l, add)
     int x, start, end, clt_l, add;
{
  int hantentmp;
  int marktmp;

  if (insert_modep ())
    {
      hantentmp = c_b->hanten;
      marktmp = c_b->t_m_start;
      c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
      c_b->t_b_st = x;
      c_b->t_b_end = x + add;
      c_b->t_m_start = x + add;
      t_redraw_move_yincod (x, start, end, clt_l);
      c_b->hanten = hantentmp;
      c_b->t_m_start = marktmp;
    }
  else
    {
      t_redraw_move (x, start, end, clt_l);
    }
  return (0);
}

int
call_t_redraw_move_1_yincod (x, start, end, clt_l, add1, add2, mode)
     int x, start, end, clt_l, add1, add2, mode;
{
  int hantentmp = 0;
  int marktmp = 0;

  if (mode)
    {
      if (mode == 2)
        {
          c_b->t_b_st = start;
          c_b->t_b_end = end;
        }
      else if (mode == 3)
        {
          marktmp = c_b->t_m_start;
          c_b->t_b_st = start;
          c_b->t_b_end = start + add1;
          c_b->t_m_start = start + add1;
        }
      else
        {
          hantentmp = c_b->hanten;
          marktmp = c_b->t_m_start;
          c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
          c_b->t_b_st = start;
          c_b->t_b_end = start + add1;
          c_b->t_m_start = start + add1;
        }
      t_redraw_move_yincod (x, start, end, clt_l);
      if (mode == 3)
        {
          c_b->t_m_start = marktmp;
        }
      else if (mode == 1)
        {
          c_b->hanten = hantentmp;
          c_b->t_m_start = marktmp;
        }
    }
  else
    {
      t_move (x + add2);
    }
  return (0);
}

int
call_t_redraw_move_2_yincod (x, start1, start2, end1, end2, clt_l, add)
     int x, start1, start2, end1, end2, clt_l, add;
{
  int hantentmp;
  int marktmp;
#ifdef  XJUTIL
  extern int touroku_mode;

  if (touroku_mode != KANJI_IN_END)
    {
#endif /* XJUTIL */
      hantentmp = c_b->hanten;
      marktmp = c_b->t_m_start;
      c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
      c_b->t_b_st = x;
      c_b->t_b_end = x + add;
      c_b->t_m_start = x + add;
      t_redraw_move_yincod (x, start1, end1, clt_l);
      c_b->hanten = hantentmp;
      c_b->t_m_start = marktmp;
#ifdef  XJUTIL
    }
  else
    {
      t_redraw_move_yincod (x, start2, end2, clt_l);
    }
#endif /* XJUTIL */
  return (0);
}

int
call_t_print_l_yincod (x, add)
     int x, add;
{
  int hantentmp;
  int marktmp;

  hantentmp = c_b->hanten;
  marktmp = c_b->t_m_start;
  c_b->hanten = 0x20 | 0x02 | 0x40 | 0x04;
  c_b->t_b_st = x;
  c_b->t_b_end = x + add;
  c_b->t_m_start = x + add;
  t_print_l ();
  c_b->hanten = hantentmp;
  c_b->t_m_start = marktmp;
  return (0);
}

int
input_yincod (s1, s2)
     unsigned int *s1, *s2;
{
  char env_s = env_state ();
  int which = 0;

  if (env_s == 'P')
    {
      which = CWNN_PINYIN;
    }
  else if (env_s == 'Z')
    {
      which = CWNN_ZHUYIN;
    }
  else
    {
      return (0);
    }
  return (cwnn_pzy_yincod (s1, s2, which));
}

int
redraw_when_chmsig_yincod ()
{
  int hantentmp;
  int marktmp;
  char env_s;

  if (env_s = env_state ())
    {
      set_cur_env (env_s);
    }
  if (insert_modep ())
    {
      hantentmp = c_b->hanten;
      marktmp = c_b->t_m_start;
      c_b->hanten = 0x40 | 0x04 | 0x20 | 0x02;
      c_b->t_b_st = c_b->t_c_p;
      c_b->t_b_end = c_b->t_c_p + 1;
      c_b->t_m_start = c_b->t_c_p + 1;
      redraw_line ();
      c_b->hanten = hantentmp;
      c_b->t_m_start = marktmp;
    }
  else
    {
      redraw_line ();
    }
  return (0);
}

int
c_top_yincod ()
{
  int hantentmp = c_b->hanten;
  int marktmp = c_b->t_m_start;
#ifdef  XJUTIL
  extern int touroku_mode;

  if (touroku_mode != KANJI_IN_END)
    {
#endif /* XJUTIL */
      c_b->hanten = 0x02 | 0x20 | 0x04 | 0x40;
      if (c_b->t_m_start < 0)
        {
          c_b->t_b_st = 0;
          c_b->t_b_end = 1;
          c_b->t_m_start = 1;
          t_redraw_move_yincod (0, 0, c_b->maxlen, 1);
        }
      else
        {
          c_b->t_b_st = c_b->t_m_start;
          c_b->t_b_end = c_b->t_m_start + 1;
          c_b->t_m_start = c_b->t_m_start + 1;
          t_redraw_move_yincod (c_b->t_b_st, c_b->t_b_st, c_b->maxlen, 1);
        }
      c_b->hanten = hantentmp;
      c_b->t_m_start = marktmp;
#ifdef  XJUTIL
    }
  else
    {
      t_redraw_move_yincod (c_b->t_m_start, c_b->t_m_start, c_b->maxlen, 0);
    }
#endif /* XJUTIL */
  return (0);
}

int
c_end_yincod ()
{
  int hantentmp = c_b->hanten;
  int marktmp = c_b->t_m_start;
#ifdef  XJUTIL
  extern int touroku_mode;

  if (touroku_mode != KANJI_IN_END)
    {
#endif /* XJUTIL */
      c_b->hanten = 0x02 | 0x20 | 0x04 | 0x40;
      c_b->t_b_st = c_b->maxlen;
      c_b->t_b_end = c_b->maxlen;
      c_b->t_m_start = c_b->maxlen;
      t_redraw_move_yincod (c_b->maxlen, c_b->t_c_p, c_b->maxlen, 1);
      kk_cursor_normal ();
      c_b->hanten = hantentmp;
      c_b->t_m_start = marktmp;
#ifdef  XJUTIL
    }
  else
    {
      t_redraw_move_yincod (c_b->maxlen, c_b->t_m_start, c_b->maxlen, 0);
    }
#endif /* XJUTIL */
  return (0);
}

int
c_end_nobi_yincod ()
{
  t_redraw_move_yincod (c_b->maxlen, c_b->t_m_start, c_b->maxlen, 0);
  return (0);
}

int
print_out_yincod (s1, s2, len)
     w_char *s1, *s2;
     int len;
{
  return (cwnn_yincod_pzy_str (s1, s2, len, ((env_state () == 'P') ? CWNN_PINYIN : CWNN_ZHUYIN)));
}

int
char_q_len_yincod (x)
     w_char x;
{
  if (((int) (x & 0x00ff) > 0xa0) && ((int) (x & 0x00ff) < 0xff) && ((int) (x >> 8) > 0xa0) && ((int) (x >> 8) < 0xff))
    return (2);
  else
    return (1);
}

int
char_len_yincod (x)
     w_char x;
{
  w_char tmp_wch[10];
  int len, i, ret_col = 0;

  len = (*print_out_func) (tmp_wch, &x, 1);
  for (i = 0; i < len; i++)
    {
      ret_col += char_q_len_yincod (tmp_wch[i]);
    }
  return (ret_col);
}

#ifndef XJUTIL
int
not_call_jl_yomi_len ()
{
  int ret = bun_data_->bun[cur_bnst_]->yomilen;
  return (ret);
}
#endif /* !XJUTIL */
#endif /* CHINESE */
