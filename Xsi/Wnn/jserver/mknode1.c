/*
 *  $Id: mknode1.c $
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
#include "de_header.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

static void lnk_sbn ();

static struct SYO_BNSETSU *free_sbn_top = NULL;
static struct free_list *free_list_sbn = NULL;
/************************************************/
/* initialize link struct SYO_BNSETSU           */
/************************************************/
int
init_sbn ()
{
  free_area (free_list_sbn);
  if (get_area (FIRST_SBN_KOSUU, sizeof (struct SYO_BNSETSU), &free_list_sbn) < 0)
    return (-1);
  lnk_sbn (free_list_sbn);
  return (0);
}

int
get_area (kosuu, size, list)
     register int kosuu;
     register int size;
     struct free_list **list;
{
  register struct free_list *area;

  if ((area = (struct free_list *) malloc (size * kosuu + sizeof (struct free_list))) == NULL)
    {
      wnn_errorno = WNN_MALLOC_INITIALIZE;
      error1 ("Cannot Malloc in get_area.\n");
      return (-1);
    }
  area->lnk = *list;
  area->num = kosuu;
  *list = area;
  return (0);
}

void
free_area (list)
     register struct free_list *list;
{
  if (list == 0)
    return;
  free_area (list->lnk);
  free (list);
}

/* free_sbn が 0 でない時に呼んだらあかんよ */
static void
lnk_sbn (list)
     struct free_list *list;
{
  register int n;
  register struct SYO_BNSETSU *wk_ptr;

  free_sbn_top = wk_ptr = (struct SYO_BNSETSU *) ((char *) list + sizeof (struct free_list));

  for (n = list->num - 1; n > 0; wk_ptr++, n--)
    wk_ptr->lnk_br = wk_ptr + 1;
  wk_ptr->lnk_br = 0;
}


/*******************************************************/
/* struct SYO_BNSETSU & ICHBNP & KANGO free エリア作成 */
/*******************************************************/
void
freesbn (sbn)                   /* struct SYO_BNSETSU を free_area へリンク */
     register struct SYO_BNSETSU *sbn;  /* クリアするノードのポインタ */
{
  if (sbn == 0)
    return;
  sbn->reference--;
  if (sbn->reference <= 0)
    {
      sbn->lnk_br = free_sbn_top;
      free_sbn_top = sbn;
    }
}

void
clr_sbn_node (sbn)
     register struct SYO_BNSETSU *sbn;
{
  if (sbn == 0)
    return;
  if ((sbn->reference) <= 1)
    {
      freesbn (sbn);
      clr_sbn_node (sbn->parent);
    }
  else
    {
      sbn->reference--;
    }
}

/******************************************/
/* SYO_BNSETSU area の獲得                */
/******************************************/
struct SYO_BNSETSU *
getsbnsp ()
{
  register struct SYO_BNSETSU *rtnptr;

  if (free_sbn_top == 0)
    {
      if (get_area (SBN_KOSUU, sizeof (struct SYO_BNSETSU), &free_list_sbn) < 0)
        return ((struct SYO_BNSETSU *) -1);
      lnk_sbn (free_list_sbn);
    }

  rtnptr = free_sbn_top;
  free_sbn_top = free_sbn_top->lnk_br;
  rtnptr->lnk_br = 0;
  rtnptr->parent = 0;
  rtnptr->son_v = 0;
  rtnptr->reference = 0;
  rtnptr->jentptr = 0;
  rtnptr->status = 0;
  rtnptr->status_bkwd = 0;
  return (rtnptr);
}
