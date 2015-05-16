/*
 *  $Id: get_kaiarea.c $
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

#include <stdio.h>
#include "commonhd.h"
#include "de_header.h"
#include "jdata.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

int
get_kaiseki_area (kana_len)
     int kana_len;
{
  char *c;
  char *area_pter;

  int maxj_len = (kana_len * sizeof (int) + 7) & 0xfffffff8;
  int jmtp_len = (kana_len * sizeof (struct jdata **) + 7) & 0xfffffff8;
  int jmt_len = (SIZE_JISHOTABLE * sizeof (struct jdata *) + 7) & 0xfffffff8;
  int jmtw_len = (SIZE_JISHOHEAP * sizeof (struct jdata) + 7) & 0xfffffff8;

  int bun_len = ((kana_len + 1) * sizeof (w_char) + 7) & 0xfffffff8;

  if ((area_pter = malloc (bun_len + jmtw_len + jmt_len + jmtp_len + maxj_len)) == NULL)
    {
      wnn_errorno = WNN_MALLOC_INITIALIZE;
      error1 ("malloc in get_kaiseki_area");
      return (-1);
    }

  maxj = (int *) area_pter;
  area_pter += maxj_len;
  for (c = (char *) maxj; c < area_pter;)
    {
      *(c++) = 0;
    }

  jmtp = (struct jdata ***) area_pter;
  area_pter += jmtp_len;
  for (c = (char *) jmtp; c < area_pter;)
    {
      *(c++) = 0;
    }

  jmt_ = (struct jdata **) area_pter;
  area_pter += jmt_len;
  jmtw_ = (struct jdata *) area_pter;
  area_pter += jmtw_len;
  jmt_end = jmt_ + SIZE_JISHOTABLE;
  jmt_ptr = jmt_end;            /* H.T. To initialize all in jmt_init */
  jmtw_end = jmtw_ + SIZE_JISHOHEAP;

  bun = (w_char *) area_pter;
  area_pter += bun_len;

  maxchg = kana_len;
  initjmt = maxchg - 1;
  bun[maxchg] = NULL;

  return (0);
}


/* サーバーが起きた時に呼ぶ
        解析ワークエリアをクリアする
        変換がエラーリターンした時にも呼んでね。*/

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
