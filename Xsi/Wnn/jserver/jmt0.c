/*
 *  $Id: jmt0.c $
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

#include "commonhd.h"
#include "jslib.h"
#include "jdata.h"
#include "kaiseki.h"
#include "de_header.h"

#include <stdio.h>


void
init_jmt (x)
     register int x;
{
  register int n;
  register struct jdata **keep_ptr;
  register struct jdata **jmt_ptr_org = jmt_ptr;        /* H.T. */

  if (x == 0)
    x = initjmt;                /* H.T. 22/12/89 */

  for (n = x; n < initjmt && (jmtp[n] == (struct jdata **) 0 || jmtp[n] == (struct jdata **) -1); n++)
    ;

  if (n < initjmt)
    {                           /* Tukatte nai nodakara, kesubeki deha... */
      keep_ptr = jmt_ptr;

      if (n == x)
        jmt_ptr = jmtp[x];
      else
        jmt_ptr = jmtp[n] + (maxj[n] - n + 1);

      for (n = 0; (jmt_ptr + n) < keep_ptr && *(jmt_ptr + n) <= (struct jdata *) 0; n++)
        ;
      if ((jmt_ptr + n) < keep_ptr)
        j_e_p = *(jmt_ptr + n);
    }
  else
    {
      jmt_ptr = jmt_;
      j_e_p = jmtw_;
    }
/* this part is added 8/18 by H.T */
  for (keep_ptr = jmt_ptr; keep_ptr < jmt_ptr_org; keep_ptr++)
    {
      *keep_ptr = NULL;
    }
}


int
jmt_set (yomi)
     register int yomi;         /* 読み文字列の先頭へのポインタ(逆順) */
{
  register int n;
  if ((n = jishobiki (&(bun[yomi]), jmt_ptr)) > 0)
    {
      jmtp[yomi] = jmt_ptr;
      jmt_ptr += n;
      maxj[yomi] = yomi + (n - 1);
    }
  else
    {
      if (n == 0)
        {
          jmtp[yomi] = 0;
          maxj[yomi] = 0;
        }
      else
        {
          jmtp[yomi] = 0;
          maxj[yomi] = 0;
        }
    }
  return (1);
}
