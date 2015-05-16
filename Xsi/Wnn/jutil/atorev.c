/*
 *  $Id: atorev.c $
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

/*
UJIS 形式を、逆変換可能形式に変換するプログラム。
*/

#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"
#include "wnn_string.h"

extern int kanjicount;
extern struct JT jt;

static void r_alloc (), make_ri2 (), make_ri1 (), set_rpter1 ();
extern int sort_func_je ();
extern void sort_if_not_sorted (), uniq_je (), output_dic_data (), sort_kanji (), upd_kanjicount ();

struct je **ptmp;

void
create_rev_dict ()
{
  r_alloc ();
  sort_if_not_sorted ();
  uniq_je (sort_func_je);
  output_dic_data ();
  make_ri2 ();
  make_ri1 (D_YOMI);
  set_rpter1 (D_YOMI);
  sort_kanji ();
  make_ri1 (D_KANJI);
  set_rpter1 (D_KANJI);
}

static void
r_alloc ()
{
  if ((jt.ri2 = (struct rind2 *) malloc ((jt.maxserial) * sizeof (struct rind2))) == NULL ||
      (jt.ri1[D_YOMI] = (struct rind1 *) malloc (jt.maxserial * sizeof (struct rind1))) == NULL ||
      (jt.ri1[D_KANJI] = (struct rind1 *) malloc (jt.maxserial * sizeof (struct rind1))) == NULL || (ptmp = (struct je **) malloc (jt.maxserial * sizeof (struct je *))) == NULL)
    {
      fprintf (stderr, "Malloc Failed\n");
      exit (1);
    }
}

static void
make_ri2 ()
{
  register int s;

  for (s = 0; s < jt.maxserial; s++)
    {
      jeary[s]->serial = s;
      jt.ri2[s].kanjipter = kanjicount;
      upd_kanjicount (s);
    }
}

static void
make_ri1 (which)
     int which;
{
  register w_char *yomi;
  w_char dummy = 0;             /* 2 byte yomi */
  w_char *pyomi;                /* maeno tangono yomi */
  register int s, t;

  yomi = &dummy;

  for (t = 0, s = 0; s < jt.maxserial; s++)
    {
      pyomi = yomi;
      yomi = (which == D_YOMI) ? jeary[s]->yomi : jeary[s]->kan;

      if (wnn_Strcmp (yomi, pyomi))
        {
          (jt.ri1[which])[t].pter = jeary[s]->serial;
          if (s)
            (jt.ri2[jeary[s - 1]->serial].next)[which] = RD_ENDPTR;
          ptmp[t] = jeary[s];
          t++;
        }
      else
        {
          if (s)
            (jt.ri2[jeary[s - 1]->serial].next)[which] = jeary[s]->serial;
        }
    }
  if (s > 0)
    {
      (jt.ri2[jeary[s - 1]->serial].next)[which] = RD_ENDPTR;
    }
  jt.maxri1[which] = t;
}

static void
set_rpter1 (which)
     int which;
{
  register int k;
  register int len;
  w_char *oyomi, *nyomi;
  /* May be a little slow, but simple! */
  int lasts[LENGTHYOMI];        /* pter_to */

  for (k = 0; k < LENGTHYOMI; k++)
    {
      lasts[k] = -1;
    }

  for (k = 0; k < jt.maxri1[which]; k++)
    {
      nyomi = (which == D_YOMI) ? ptmp[k]->yomi : ptmp[k]->kan;
      len = wnn_Strlen (nyomi);
      lasts[len] = k;
      for (len--; len; len--)
        {
          if (lasts[len] >= 0)
            {
              oyomi = (which == D_YOMI) ? ptmp[lasts[len]]->yomi : ptmp[lasts[len]]->kan;
              if (wnn_Substr (oyomi, nyomi))
                {
                  (jt.ri1[which])[k].pter1 = lasts[len];
                  break;
                }
            }
        }
      if (len == 0)
        (jt.ri1[which])[k].pter1 = RD_ENDPTR;
    }
}

void
output_ri (ofpter)
     FILE *ofpter;
{
  fwrite (jt.ri1[D_YOMI], sizeof (struct rind1), jt.maxri1[D_YOMI], ofpter);
  fwrite (jt.ri1[D_KANJI], sizeof (struct rind1), jt.maxri1[D_KANJI], ofpter);
  fwrite (jt.ri2, sizeof (struct rind2), jt.maxri2, ofpter);
}
