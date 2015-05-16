/*
 *  $Id: jmt0.c,v 1.4 2003/05/11 18:41:44 hiroo Exp $
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

#include <stdio.h>

#include "commonhd.h"
#include "de_header.h"
#include "jslib.h"
#include "jdata.h"
#include "kaiseki.h"

void
init_jmt (void)
{
  struct jdata **keep_ptr;
  struct jdata **jmt_ptr_org = jmt_ptr;

  jmt_ptr = jmt_;
  j_e_p = jmtw_;

  for (keep_ptr = jmt_ptr; keep_ptr < jmt_ptr_org; keep_ptr++)
    {
      *keep_ptr = NULL;
    }
}


int
jmt_set (int yomi)
	/* yomi: 読み文字列の先頭へのポインタ(逆順) */
{
  int n;
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

