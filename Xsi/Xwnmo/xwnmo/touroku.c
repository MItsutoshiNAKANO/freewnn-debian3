/*
 * $Id: touroku.c,v 1.2.2.1 1999/02/08 08:08:57 yamasita Exp $
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
#include "xim.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "ext.h"

static void
make_touroku_buffer (bnst, cbup)
     int bnst;
     ClientBuf *cbup;
{
  int k, l;
  w_char *bp;
  w_char *buffer_end;

  buffer_end = c_b->buffer + c_b->buflen - 1;
  bp = c_b->buffer;
  for (k = bnst; k < jl_bun_suu (bun_data_); k++)
    {
      if (k < maxbunsetsu)
        touroku_bnst[k] = bp - c_b->buffer;
      l = jl_get_kanji (bun_data_, k, k + 1, bp);
      bp += l;
      if (bp >= buffer_end)
        {
          k--;
          bp -= l;
          goto GOT_IT;
        }
    }
  if (cbup->maxlen == 0)
    {
      get_end_of_history (bp);
    }
  else
    {
      Strncpy (bp, cbup->buffer + bunsetsu[jl_bun_suu (bun_data_)], cbup->maxlen - bunsetsu[jl_bun_suu (bun_data_)]);
      *(bp + cbup->maxlen - bunsetsu[jl_bun_suu (bun_data_)]) = 0;
    }

GOT_IT:
  if (k < maxbunsetsu)
    {
      touroku_bnst[k++] = bp - c_b->buffer;
      touroku_bnst_cnt = k;
    }
  else
    {
      touroku_bnst[maxbunsetsu - 1] = bp - c_b->buffer;
      touroku_bnst_cnt = maxbunsetsu;
    }
}

int
touroku ()
{
  w_char buffer[1024];
  ClientBuf *c_btmp, c_b1;

#ifdef  USING_XJUTIL
  c_btmp = c_b;
  c_b = &c_b1;

  c_b->buffer = buffer;
  c_b->buflen = 1024;

  make_touroku_buffer (0, c_btmp);
  xw_touroku (touroku_bnst[cur_bnst_]);
  c_b = c_btmp;
#endif /* USING_XJUTIL */
  return (0);
}
