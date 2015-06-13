/*
 *  $Id: get_kaiarea.c,v 1.8 2013/09/02 11:01:39 itisango Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2003
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <sys/types.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#endif /* STDC_HEADERS */

#include "commonhd.h"
#include "de_header.h"
#include "jdata.h"
#define GLOBAL_VALUE_DEFINE 1
#include "kaiseki.h"
#undef  GLOBAL_VALUE_DEFINE

/*
 * get_kaiseki_area: take memory areas for analysing.
 * return value: fail = 0, success != 0
 */
int
get_kaiseki_area (size_t kana_len)
{
  if (NULL == (maxj = (int *) calloc (kana_len, sizeof(int))))
    {
      wnn_errorno = WNN_MALLOC_INITIALIZE;
      log_debug ("malloc failed in maxj.");
      return (0);
    }

  if (NULL == (jmtp = (struct jdata ***) calloc (kana_len, sizeof (struct jdata **))))
    {
      wnn_errorno = WNN_MALLOC_INITIALIZE;
      log_debug ("malloc failed in jmtp.");
      return (0);
    }

  if (NULL == (jmt_ = (struct jdata **) calloc (SIZE_JISHOTABLE, sizeof (struct jdata *))))
    {
      wnn_errorno = WNN_MALLOC_INITIALIZE;
      log_debug ("malloc failed in jmt_.");
      return (0);
    }

  if (NULL == (jmtw_ = (struct jdata *) calloc (SIZE_JISHOHEAP, sizeof (struct jdata))))
    {
      wnn_errorno = WNN_MALLOC_INITIALIZE;
      log_debug ("malloc failed in jmtw_.");
      return (0);
    }

  jmt_end = jmt_ + SIZE_JISHOTABLE;
  jmt_ptr = jmt_end;		/* H.T. To initialize all in jmt_init */
  jmtw_end = jmtw_ + SIZE_JISHOHEAP;

  if (NULL == (bun = (w_char *) calloc ((kana_len + 1), sizeof (w_char))))
    {
      wnn_errorno = WNN_MALLOC_INITIALIZE;
      log_debug ("malloc failed in bun.");
      return (0);
    }

  maxchg = kana_len;

  return (1);
}


/*
 * サーバーが起きた時に呼ぶ
 * 解析ワークエリアをクリアする
 * 変換がエラーリターンした時にも呼んでね。
 */

void
init_work_areas ()
{
  init_ichbnp ();
  init_bzd ();
  init_sbn ();
  init_jktdbn ();
  init_jktsbn ();
  init_jktsone ();
}

