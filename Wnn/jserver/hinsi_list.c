/*
 *  $Id: hinsi_list.c,v 1.5 2002/05/12 22:51:16 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002
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
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#endif /* STDC_HEADERS */

#include "commonhd.h"
#include "wnn_config.h"
#include "de_header.h"
#include "wnnerror.h"
#include "jslib.h"
#include "hinsi_file.h"
#include "jdata.h"

static int Strtime (), get_a_word ();

void
make_hinsi_list (jtl)
     struct JT *jtl;
{
  int k, state, cnt = 0, wc;
  w_char *j, *d, *start;

  if (jtl->maxhinsi_list == 0)
    {
      jtl->node = (struct wnn_hinsi_node *) malloc (0);
      jtl->maxnode = 0;
      return;
    }
  jtl->maxnode = Strtime (jtl->hinsi_list, jtl->maxhinsi_list, NODE_CHAR);
  jtl->node = (struct wnn_hinsi_node *) malloc (jtl->maxnode * sizeof (struct wnn_hinsi_node) + jtl->maxhinsi_list * sizeof (w_char));
  d = (w_char *) (jtl->node + jtl->maxnode);
  if (jtl->hinsi_list == NULL)
    return;
  j = jtl->hinsi_list;
  k = -1;
  for (;;)
    {
      wc = get_a_word (j, &start, &state);
      if (state == 2 && wc == 0)
        {
          break;
        }
      Strncpy (d, start, wc);
      d[wc] = 0;
      if (state == 2)
        {
          cnt++;
          break;
        }
      if (state == 1)
        {
          if (k >= 0)
            jtl->node[k].kosuu = cnt;
          cnt = 0;
          k++;
          jtl->node[k].name = d;
          jtl->node[k].son = d + wc + 1;
        }
      else
        {
          cnt++;
        }
      d += wc + 1;
      j = start + wc + 1;
    }
  if (k >= 0)
    jtl->node[k].kosuu = cnt;
}


static int
Strtime (w, wc, c)
     w_char *w;
     int wc;
     w_char c;
{
  int ret = 0;
  for (; wc; wc--, w++)
    {
      if (*w == c)
        ret++;
    }
  return (ret);
}

static int
get_a_word (d, startp, statep)
     w_char *d, **startp;
     int *statep;
{
  w_char *d0 = d;
  int c, wc;
  for (;;)
    {
      if ((c = *d++) == 0)
        {
          *statep = 2;
          *startp = d0;
          return (0);
        }
      if (c == IGNORE_CHAR1 || c == IGNORE_CHAR2 || c == CONTINUE_CHAR || c == '\n' || c == HINSI_SEPARATE_CHAR)
        {
          continue;
        }
      if (c == COMMENT_CHAR)
        {
          for (; (*d++ != '\n'););
          continue;
        }
      break;
    }
  *startp = d - 1;
  wc = 1;
  for (;; wc++)
    {
      if ((c = *d++) == 0)
        {
          *statep = 2;
          return (wc);
        }
      if (c == IGNORE_CHAR1 || c == IGNORE_CHAR2 || c == CONTINUE_CHAR || c == '\n' || c == HINSI_SEPARATE_CHAR)
        {
          *statep = 0;
          return (wc);
        }
      else if (c == NODE_CHAR)
        {
          *statep = 1;
          return (wc);
        }
    }
}

int
has_dict (n, m, w)
     struct wnn_hinsi_node *n;
     int m;
     w_char *w;
{
  int j;
  for (; m; m--)
    {
      if (Strcmp (n[m].name, w) == 0)
        for (j = n[m].kosuu; j; j--)
          {
            if (Strcmp (n[m].son[j], w) == 0)
              return (1);
          }
    }
  return (0);
}

int
hinsi_table_set (jtl, table)
     struct JT *jtl;
     w_char *table;
{
  /*
     int len;
   */

  free (jtl->node);
  free (jtl->hinsi_list);

  jtl->maxhinsi_list = Strlen (table);

  jtl->hinsi_list = (w_char *) malloc (jtl->maxhinsi_list * sizeof (w_char));
  Strcpy (jtl->hinsi_list, table);
  make_hinsi_list (jtl);
  jtl->dirty = 1;
  return (0);
}
