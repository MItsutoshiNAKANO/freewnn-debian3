/*
 *  $Id: utilbdic.c,v 1.4 2004/07/19 18:24:26 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2004
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

/*
  Binary (Updatable, Stable) dictionary common routine.
*/

/* extern functions are
  output_header(ofpter)
  output_hindo(ofpter)
  output_hinsi(ofpter)
*/

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

int wnnerror;

struct JT jt;
struct wnn_file_head file_head;

extern char *passwd;

output_hinsi (FILE* ofpter)
{
  int i;

  for (i = 0; i < jt.maxserial; i++)
    {
      put_short (ofpter, jeary[i]->hinsi);
    }
/*
  if(i & 0x1){
    put_short(ofpter,0);
  }
*/
}

input_hinsi (FILE* ifpter)
{
  int i;
  unsigned short s;

  for (i = 0; i < jt.maxserial; i++)
    {
      if (get_short (&s, ifpter) == -1)
        return (-1);
      jeary[i]->hinsi = s;
    }
/*
  if(i & 0x1){
    get_short(ifpter);
  }
*/
  return (0);
}

#ifdef nodef
revdic_jt (struct JT* jtp, int match, char* hostart, struct uind1* tary)
{
/* Must reverse hinsi?????? */

  if (jtp->syurui == WNN_UD_DICT)
    {
      rev_ud_hontai (hostart, jtp->maxhontai, match);
      rev_ud_table (tary, jtp->maxtable, match);
    }
  else
    {
      rev_sd_hontai (hostart, match);
    }
  return (0);
}
#endif
