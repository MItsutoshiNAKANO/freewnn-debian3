/*
 *  $Id: do_henkan1.c,v 1.7 2003/05/11 18:41:49 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2003
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

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#include "demcom.h"
#include "commonhd.h"
#include "de_header.h"
#include "jdata.h"
#include "fzk.h"

#include "kaiseki.h"

#define FZK_L 256
#ifdef  putwchar
#undef  putwchar
#endif

static int make_buns_par ();

static int bun_pter_;

static int
henkan_setup ()
{
  register int index;
  register w_char c;

  wnn_loadhinsi (hinsi_file_name);
#ifdef  CONVERT_from_TOP
  index = 1;
#else
  index = maxchg - 1;
#endif
  while ((c = get2_cur ()) != 0)
    {
#ifdef  DEBUG
      putwchar (c);
#endif
      if (make_buns_par (c, index) == -1)
        {
          while ((c = get2_cur ()) != 0);       /* dummy get */
          return (-1);
        }
#ifdef  CONVERT_from_TOP
      index++;
#else
      index--;
#endif
    }
#ifdef  CONVERT_from_TOP
  bun_pter_ = index;
#else
  bun_pter_ = index + 1;
#endif
  init_jmt ();
#ifdef  DEBUG
  wsputs ("\n");
#endif
  return (0);
}

static void
get_fzk_vec (hinsi, fzk, fzk_size, vec, vec1)
     register int *hinsi;
     register w_char *fzk;
     register size_t fzk_size;
     register int *vec;
     register int *vec1;
{
  *hinsi = get4_cur ();
  getws_cur (fzk, fzk_size);
  *vec = get4_cur ();
  *vec1 = get4_cur ();
}

#ifdef CHINESE
static void
do_question (daip, zenp)
     int daip, zenp;
{
  int b_suu_;

  b_suu_ = jishobiki_b (1, bun_pter_, 0);
  if (b_suu_ < 0)
    {
      error_ret ();             /* ERROR */
      return;
    }
  if (zenp == WNN_BUN)
    b_suu_ = (b_suu_ == 0) ? 0 : 1;
  if (daip == WNN_DAI)
    ret_daiB (1, bun_pter_, b_suu_, zenp);
  else
    ret_B (1, bun_pter_, b_suu_, zenp);
  putc_purge ();
}
#endif

/* RENBUN */
void
do_kanren ()
{
  int end, eid;
  int vec;
  int vec1;
  int vec2;
  w_char fzk[FZK_L];
  struct DSD_DBN *d_list;
  register int b_suu_;
  int hinsi;
  int err;

  eid = get4_cur ();
  c_env = env[eid];
  end = maxchg - 1;
  err = henkan_setup ();
  get_fzk_vec (&hinsi, fzk, FZK_L, &vec, &vec1);
  vec2 = get4_cur ();
  if (err == -1)
    {
      error_ret ();             /* ERROR */
      return;
    }

#ifdef CHINESE
  if (is_bwnn_rev_dict ())
    {
      do_question (WNN_DAI, WNN_BUN);
      return;
    }
#endif /* CHINESE */

  b_suu_ = renbn_kai (
#ifdef  CONVERT_from_TOP
                       1, bun_pter_,
#else
                       bun_pter_, end + 1,
#endif
                       hinsi,
#ifndef NO_FZK
                       fzk,
#endif
                       vec, vec1, vec2, c_env->nbun, c_env->nshobun, &d_list);
  if (b_suu_ < 0)
    {
      error_ret ();             /* ERROR */
      return;
    }
  ret_dai (d_list, b_suu_);
  putc_purge ();
#ifdef  DEBUG
  print_dlist (d_list, b_suu_);
#endif
}

/* TANBUN (dai)*/
void
do_kantan_dai ()
{
  int end, eid;
  int vec;
  int vec1;
  w_char fzk[FZK_L];
  struct DSD_DBN *d_list;
  register int b_suu_;
  int hinsi;
  int err;

  eid = get4_cur ();
  c_env = env[eid];
  end = maxchg - 1;
  err = henkan_setup ();
  get_fzk_vec (&hinsi, fzk, FZK_L, &vec, &vec1);
  if (err == -1)
    {
      error_ret ();             /* ERROR */
      return;
    }

#ifdef CHINESE
  if (is_bwnn_rev_dict ())
    {
      do_question (WNN_DAI, WNN_BUN);
      return;
    }
#endif /* CHINESE */
  b_suu_ = tan_dai (
#ifdef  CONVERT_from_TOP
                     1, bun_pter_,
#else
                     bun_pter_, end + 1,
#endif
                     hinsi,
#ifndef NO_FZK
                     fzk,
#endif
                     vec, vec1, c_env->nshobun, &d_list);
  if (b_suu_ < 0)
    {
      error_ret ();             /* ERROR */
      return;
    }
  ret_dai (d_list, b_suu_);
  putc_purge ();
#ifdef  DEBUG
  print_dlist (d_list, b_suu_);
#endif
}

/* TANBUN (sho)*/
void
do_kantan_sho ()
{
  int end, eid;
  int vec;
  int vec1;
  w_char fzk[FZK_L];
  struct DSD_SBN *d_list1;
  register int b_suu_;
  int hinsi;
  int err;

  eid = get4_cur ();
  c_env = env[eid];
  end = maxchg - 1;
  err = henkan_setup ();
  get_fzk_vec (&hinsi, fzk, FZK_L, &vec, &vec1);
  if (err == -1)
    {
      error_ret ();             /* ERROR */
      return;
    }

#ifdef CHINESE
  if (is_bwnn_rev_dict ())
    {
      do_question (WNN_SHO, WNN_BUN);
      return;
    }
#endif /* CHINESE */
  b_suu_ = tan_syo (
#ifdef  CONVERT_from_TOP
                     1, bun_pter_,
#else
                     bun_pter_, end + 1,
#endif
                     hinsi,
#ifndef NO_FZK
                     fzk,
#endif
                     vec, vec1, &d_list1);
  if (b_suu_ < 0)
    {
      error_ret ();             /* ERROR */
      return;
    }
  ret_sho (d_list1, b_suu_);
  putc_purge ();
#ifdef  DEBUG
  print_dlist1 (d_list1, b_suu_);
#endif
}

/* JIKOUHO (dai)*/
void
do_kanzen_dai ()
{
  int end, eid;
  int vec;
  int vec1;
  w_char fzk[FZK_L];
  struct DSD_DBN *d_list;
  register int b_suu_;
  int hinsi;
  int err;

  eid = get4_cur ();
  c_env = env[eid];
  end = maxchg - 1;
  err = henkan_setup ();
  get_fzk_vec (&hinsi, fzk, FZK_L, &vec, &vec1);
  if (err == -1)
    {
      error_ret ();             /* ERROR */
      return;
    }

#ifdef CHINESE
  if (is_bwnn_rev_dict ())
    {
      do_question (WNN_DAI, WNN_ZENKOUHO);
      return;
    }
#endif /* CHINESE */
  b_suu_ = jkt_get_dai (
#ifdef  CONVERT_from_TOP
                         1, bun_pter_,
#else
                         bun_pter_, end + 1,
#endif
                         hinsi,
#ifndef NO_FZK
                         fzk,
#endif
                         vec, vec1, c_env->nshobun, &d_list);
  if (b_suu_ < 0)
    {
      error_ret ();             /* ERROR */
      return;
    }
  ret_dai (d_list, b_suu_);
  putc_purge ();
#ifdef  DEBUG
  print_dlist (d_list, b_suu_);
#endif
}

/* JIKOUHO (sho)*/
void
do_kanzen_sho ()
{
  int end, eid;
  int vec;
  int vec1;
  w_char fzk[FZK_L];
  struct DSD_SBN *d_list1;
  register int b_suu_;
  int hinsi;
  int err;

  eid = get4_cur ();
  c_env = env[eid];
  end = maxchg - 1;
  err = henkan_setup ();
  get_fzk_vec (&hinsi, fzk, FZK_L, &vec, &vec1);
  if (err == -1)
    {
      error_ret ();             /* ERROR */
      return;
    }

#ifdef CHINESE
  if (is_bwnn_rev_dict ())
    {
      do_question (WNN_SHO, WNN_ZENKOUHO);
      return;
    }
#endif /* CHINESE */
  b_suu_ = jkt_get_syo (
#ifdef  CONVERT_from_TOP
                         1, bun_pter_,
#else
                         bun_pter_, end + 1,
#endif
                         hinsi,
#ifndef NO_FZK
                         fzk,
#endif
                         vec, vec1, &d_list1);
  if (b_suu_ < 0)
    {
      error_ret ();             /* ERROR */
      return;
    }
#ifdef  DEBUG
  print_dlist1 (d_list1, b_suu_);
#endif
  ret_sho (d_list1, b_suu_);
  putc_purge ();
}

static int
make_buns_par (c, at_index)     /* make bun and bun     */
     register w_char c;
     register int at_index;
{
  if ((at_index >= maxchg) || (at_index <= 0))
    {
      wnn_errorno = WNN_LONG_MOJIRETSU;
      return (-1);
    }
  bun[at_index] = c;

  jmtp[at_index] = (struct jdata **) -1;        /* 辞書をまだ引いていない */
  return (0);
}

/* get kanji and put it in  kouho*/
void
Get_knj (jentptr, u, kouho, oy, oyl)
     int u;
     register struct jdata *jentptr;
     w_char *kouho, *oy;
     int oyl;
{
  UCHAR *kptr;
  register struct JT *jtl;

  jtl = (struct JT *) (files[dic_table[jentptr->jishono].body].area);
  kptr = jtl->kanji + jentptr->kanji1;
  if (jentptr->which == D_YOMI)
    {
      Get_knj1 (kptr, oy, oyl, jentptr->kanji2 + u, kouho, NULL, NULL);
    }
  else
    {
      Get_knj1 (kptr, oy, oyl, jentptr->kanji2 + u, NULL, kouho, NULL);
    }
}

#ifdef  no_def
void
get_yomi (jentptr, u, kouho)    /* get yomi and put it in  kouho */
     int u;
     register struct jdata *jentptr;
     w_char *kouho;
{
  UCHAR *kptr;
  register struct JT *jtl;

  jtl = (struct JT *) (files[dic_table[jentptr->jishono].body].area);
  kptr = jtl->kanji + jentptr->kanji1;
  Get_knj2 (kptr, jentptr->kanji2 + u, NULL, kouho, NULL);
}
#endif

/* get comment and put it in  kouho*/
void
get_knj_com (jentptr, u, kouho, com)
     int u;
     register struct jdata *jentptr;
     w_char *kouho;             /* kouho must be ended with NULL.       */
     w_char *com;
{
  register UCHAR *kptr;
  struct JT *jtl;

  jtl = (struct JT *) (files[dic_table[jentptr->jishono].body].area);
  kptr = jtl->kanji + jentptr->kanji1;
  Get_knj2 (kptr, jentptr->kanji2 + u, kouho, NULL, com);
}
