/*
 *  $Id: jikouho.c,v 1.4 2006/02/11 09:51:44 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2006
 *
 * Maintainer:  FreeWnn Project
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <string.h>
#else
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif /* STDC_HEADERS */

#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"         /* defines of CHMSIG, NISEBP */
#include "buffer.h"



/* 次候補を一つずつ表示する関数
num = 1 のとき次候補を、num = -1 のとき前候補を表示する */
int
jikouho (num)
     int num;
{
  int tmp;
  int mae_p;

  if (!isconect_jserver ())
    {
      throw_c (0);
      clr_line_all ();
      change_to_insert_mode ();
      c_b->t_m_start = 0;
      cur_bnst_ = 0;
      call_t_print_l (c_b->t_c_p, 1);
      if (bun_data_)
        jl_kill (bun_data_, 0, -1);
      return (-1);
    }
  /* 注目している文節の次候補がまだ用意されていない時は
     まずそれを用意する。 */
  jl_env_set (bun_data_, bunsetsu_env[cur_bnst_]);
  if (bunsetsu_env[cur_bnst_] == bunsetsu_env[cur_bnst_ + 1])
    mae_p = WNN_USE_ZENGO;
  else
    mae_p = WNN_USE_MAE;
  if (jl_zenkouho_bun (bun_data_) != cur_bnst_ || jl_zenkouho_daip (bun_data_))
    {
      if (jl_zenkouho (bun_data_, cur_bnst_, mae_p, WNN_UNIQ) < 0)
        {
          errorkeyin ();
          t_print_l ();
          return (-1);
        }
    }

  if (num == 1)
    jl_next (bun_data_);
  else if (num == -1)
    jl_previous (bun_data_);
  else
    return (-1);
  make_kanji_buffer (cur_bnst_);
  tmp = c_b->t_b_st;
  c_b->t_m_start = bunsetsuend[cur_bnst_];
  c_b->t_b_st = bunsetsu[dai_top (bun_data_, cur_bnst_)];       /* ADD KURI */
  c_b->t_b_end = bunsetsuend[dai_end (bun_data_, cur_bnst_) - 1];
  t_redraw_move (bunsetsu[cur_bnst_], min (tmp, c_b->t_b_st), c_b->maxlen, 1);
  return (0);
}

/* 次候補を一つずつ表示する関数
num = 1 のとき次候補を、num = -1 のとき前候補を表示する */
int
jikouho_dai (num)
     int num;
{
  int tmp;
  int mae_p;

  if (!isconect_jserver ())
    {
      throw_c (0);
      clr_line_all ();
      change_to_insert_mode ();
      c_b->t_m_start = 0;
      cur_bnst_ = 0;
      call_t_print_l (c_b->t_c_p, 1);
      if (bun_data_)
        jl_kill (bun_data_, 0, -1);
      return (-1);
    }
  /* 注目している文節の次候補がまだ用意されていない時は
     まずそれを用意する。 */
  jl_env_set (bun_data_, bunsetsu_env[cur_bnst_]);
  if (bunsetsu_env[cur_bnst_] == bunsetsu_env[dai_end (bun_data_, cur_bnst_)])
    mae_p = WNN_USE_ZENGO;
  else
    mae_p = WNN_USE_MAE;
  if (jl_zenkouho_bun (bun_data_) != cur_bnst_ || !jl_zenkouho_daip (bun_data_))
    {
      if (jl_zenkouho_dai (bun_data_, cur_bnst_, dai_end (bun_data_, cur_bnst_), mae_p, WNN_UNIQ) < 0)
        {
          errorkeyin ();
          t_print_l ();
          return (-1);
        }
    }

  if (num == 1)
    jl_next_dai (bun_data_);
  else if (num == -1)
    jl_previous_dai (bun_data_);
  else
    return (-1);
  make_kanji_buffer (cur_bnst_);
  for (tmp = cur_bnst_ + 1; tmp < jl_bun_suu (bun_data_); tmp++)
    bunsetsu_env[tmp] = jl_env_get (bun_data_);
  tmp = c_b->t_b_st;
  c_b->t_m_start = bunsetsuend[cur_bnst_];
  c_b->t_b_st = bunsetsu[dai_top (bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end (bun_data_, cur_bnst_) - 1];
  t_redraw_move (bunsetsu[cur_bnst_], min (tmp, c_b->t_b_st), c_b->maxlen, 1);
  return (0);
}

int
jikouho_c ()
{
  jikouho (1);
  return (0);
}

int
zenkouho_c ()
{
  jikouho (-1);
  return (0);
}

int
jikouho_dai_c ()
{
  jikouho_dai (1);
  return (0);
}

int
zenkouho_dai_c ()
{
  jikouho_dai (-1);
  return (0);
}

int
select_jikouho ()
{
  if (select_jikouho1 (WNN_SHO) == -1)
    {
      t_print_l ();
    }
  else
    {
      redraw_line ();
    }
  return (0);
}

int
select_jikouho_dai ()
{
  if (select_jikouho1 (WNN_DAI) == -1)
    {
      t_print_l ();
    }
  else
    {
      redraw_line ();
    }
  return (0);
}

int
dai_top (buf, bnst)
     struct wnn_buf *buf;
     int bnst;
{
  if (bnst >= jl_bun_suu (buf))
    return (jl_bun_suu (buf));
  for (; bnst >= 0 && !jl_dai_top (buf, bnst); bnst--);
  return (bnst);
}

int
dai_end (buf, bnst)
     struct wnn_buf *buf;
     int bnst;
{
  for (bnst++; bnst < jl_bun_suu (buf) && !jl_dai_top (buf, bnst); bnst++);
  return (bnst);
}

int
select_jikouho1 (daip)
     int daip;
{
  char *j_retu[MAXJIKOUHO];
  char jikouho_heap[MAXJIKOUHO * 10];
  int c;
  int tmp;

  if (!isconect_jserver ())
    {
      throw_c (0);
      clr_line_all ();
      change_to_insert_mode ();
      c_b->t_m_start = 0;
      cur_bnst_ = 0;
      call_t_print_l (c_b->t_c_p, 1);
      if (bun_data_)
        jl_kill (bun_data_, 0, -1);
      return (-1);
    }

  if (jl_zenkouho_bun (bun_data_) != cur_bnst_ || jl_zenkouho_daip (bun_data_) != daip)
    {
      jl_env_set (bun_data_, bunsetsu_env[cur_bnst_]);
      if (daip)
        {
          if (jl_zenkouho_dai (bun_data_, cur_bnst_, dai_end (bun_data_, cur_bnst_), WNN_USE_ZENGO, WNN_UNIQ) == -1)
            {
              errorkeyin ();
              t_print_l ();
              return (-1);
            }
        }
      else
        {
          if (jl_zenkouho (bun_data_, cur_bnst_, WNN_USE_ZENGO, WNN_UNIQ) == -1)
            {
              errorkeyin ();
              t_print_l ();
              return (-1);
            }
        }
    }
  if (make_jikouho_retu (jikouho_heap, MAXJIKOUHO * 10, j_retu, MAXJIKOUHO) == -1)
    {
      print_msg_getc (MSG_GET (5));
      /*
         print_msg_getc("候補が多過ぎて次候補が取り出せません。(如何)");
       */
      return (-1);
    }

  if ((c = select_one_element (j_retu, jl_zenkouho_suu (bun_data_), jl_c_zenkouho (bun_data_), "", 0, 0, main_table[4])) == -1)
    {
      return (-1);
    }
  if (daip == WNN_SHO)
    {
      jl_set_jikouho (bun_data_, c);
    }
  else
    {
      jl_set_jikouho_dai (bun_data_, c);
    }
  make_kanji_buffer (cur_bnst_);
  for (tmp = cur_bnst_ + 1; tmp < jl_bun_suu (bun_data_); tmp++)
    bunsetsu_env[tmp] = jl_env_get (bun_data_);
  c_b->t_m_start = bunsetsuend[cur_bnst_];
  c_b->t_c_p = bunsetsu[cur_bnst_];
  c_b->t_b_st = bunsetsu[dai_top (bun_data_, cur_bnst_)];
  c_b->t_b_end = bunsetsuend[dai_end (bun_data_, cur_bnst_) - 1];
  return (0);
}


int
make_jikouho_retu (h, h_m, r, r_m)
     char *h;
     int h_m;
     int r_m;
     char *r[];
{
  int k;
  char *c = h;
  w_char tmp[512];
  char tmp1[512];

  if (r_m <= jl_zenkouho_suu (bun_data_))
    return (-1);

  for (k = 0; k < jl_zenkouho_suu (bun_data_); k++)
    {
      r[k] = c;
      tmp[0] = 0;
      jl_get_zenkouho_kanji (bun_data_, k, tmp);
      if (print_out_func)
        (*print_out_func) (tmp, tmp, Strlen (tmp));
      wnn_delete_w_ss2 (tmp, Strlen (tmp));

      sStrcpy (tmp1, tmp);
      if (c + strlen (tmp1) + 1 >= h_m + h)
        {
          return (-1);
        }
      set_escape_code (tmp1);
      strcpy (c, tmp1);
      c += strlen (tmp1) + 1;
    }
  return (0);
}

void
set_escape_code (x)
     char *x;
{
  int m;
  char buf[512];
  char *tmp;

  for (m = 0, tmp = x; *tmp; tmp++)
    {
      if (ESCAPE_CHAR ((*tmp & 0xff)))
        {
          buf[m++] = '^';
          if (*tmp == 0x7f)
            buf[m++] = '?';
          else
            buf[m++] = (*tmp + 'A' - 1);
        }
      else
        {
          buf[m++] = *tmp;
        }
    }
  buf[m] = NULL;
  strcpy (x, buf);
}
