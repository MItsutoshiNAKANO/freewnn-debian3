/*
 *  $Id: mknode2.c $
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

static void lnk_jkdbn (), lnk_jksbn (), lnk_jksone (), freejktsone ();

static struct JKT_DBN *free_jkdbn_top = NULL;
static struct free_list *free_list_jkdbn = NULL;
static struct JKT_SBN *free_jksbn_top = NULL;
static struct free_list *free_list_jksbn = NULL;
static struct JKT_SONE *free_jksone_top = NULL;
static struct free_list *free_list_jksone = NULL;

/************************************************/
/* initialize link struct JKT_SONE           */
/************************************************/
int
init_jktdbn ()
{
  free_area (free_list_jkdbn);
  if (get_area (FIRST_JKDBN_KOSUU, sizeof (struct JKT_DBN), &free_list_jkdbn) < 0)
    return (-1);
  lnk_jkdbn (free_list_jkdbn);
  return (0);
}

/* free_jkdbn が 0 でない時に呼んだらあかんよ */
static void
lnk_jkdbn (list)
     struct free_list *list;
{
  register int n;
  register struct JKT_DBN *wk_ptr;

  free_jkdbn_top = wk_ptr = (struct JKT_DBN *) ((char *) list + sizeof (struct free_list));

  for (n = list->num - 1; n > 0; wk_ptr++, n--)
    wk_ptr->lnk_br = wk_ptr + 1;
  wk_ptr->lnk_br = 0;
}

int
init_jktsbn ()
{
  free_area (free_list_jksbn);
  if (get_area (FIRST_JKSBN_KOSUU, sizeof (struct JKT_SBN), &free_list_jksbn) < 0)
    return (-1);
  lnk_jksbn (free_list_jksbn);
  return (0);
}

/* free_jksbn が 0 でない時に呼んだらあかんよ */
static void
lnk_jksbn (list)
     struct free_list *list;
{
  register int n;
  register struct JKT_SBN *wk_ptr;

  free_jksbn_top = wk_ptr = (struct JKT_SBN *) ((char *) list + sizeof (struct free_list));

  for (n = list->num - 1; n > 0; wk_ptr++, n--)
    wk_ptr->lnk_br = wk_ptr + 1;
  wk_ptr->lnk_br = 0;
}

int
init_jktsone ()
{
  free_area (free_list_jksone);
  if (get_area (FIRST_JKSONE_KOSUU, sizeof (struct JKT_SONE), &free_list_jksone) < 0)
    return (-1);
  lnk_jksone (free_list_jksone);
  return (0);
}

/* free_jksone_top が 0 でない時に呼んだらあかんよ */
static void
lnk_jksone (list)
     struct free_list *list;
{
  register int n;
  register struct JKT_SONE *wk_ptr;

  free_jksone_top = wk_ptr = (struct JKT_SONE *) ((char *) list + sizeof (struct free_list));

  for (n = list->num - 1; n > 0; wk_ptr++, n--)
    wk_ptr->lnk_br = wk_ptr + 1;
  wk_ptr->lnk_br = 0;
}

/*******************************************************/
/* struct JKT_SONE & JKT_SBN free エリア作成 */
/*******************************************************/
void
freejktdbn (dbn)                /* struct JKT_SBN を free_area へリンク */
     register struct JKT_DBN *dbn;      /* クリアするノードのポインタ */
{
  if (dbn == 0)
    return;

  freejktsbn (dbn->sbn);
  dbn->lnk_br = free_jkdbn_top;
  free_jkdbn_top = dbn;
}

void
freejktsbn (sbn)                /* struct JKT_SBN を free_area へリンク */
     struct JKT_SBN *sbn;       /* クリアするノードのポインタ */
{
  register struct JKT_SONE *sone;
  register struct JKT_SONE *next;
  if (sbn == 0)
    return;
  sbn->reference--;
  if (sbn->reference <= 0)
    {
      for (sone = sbn->sbn; sone != 0;)
        {
          next = sone->lnk_br;
          freejktsone (sone);
          sone = next;
        }
      sbn->reference = 0;
      sbn->lnk_br = free_jksbn_top;
      free_jksbn_top = sbn;
      freejktsbn (sbn->parent);
    }
}

static void
freejktsone (sone)              /* struct JKT_SONE を free_area へリンク */
     register struct JKT_SONE *sone;    /* クリアするノードのポインタ */
{
  if (sone == 0)
    return;
  sone->lnk_br = free_jksone_top;
  free_jksone_top = sone;
}

/******************************************/
/* JKT_SBN JKT_SONE area の獲得           */
/******************************************/
struct JKT_DBN *
getjktdbn ()
{
  register struct JKT_DBN *rtnptr;

  if (free_jkdbn_top == 0)
    {
      if (get_area (JKDBN_KOSUU, sizeof (struct JKT_DBN), &free_list_jkdbn) < 0)
        return ((struct JKT_DBN *) -1);
      lnk_jkdbn (free_list_jkdbn);
    }

  rtnptr = free_jkdbn_top;
  free_jkdbn_top = free_jkdbn_top->lnk_br;
  rtnptr->lnk_br = 0;
  return (rtnptr);
}

struct JKT_SBN *
getjktsbn ()
{
  register struct JKT_SBN *rtnptr;

  if (free_jksbn_top == 0)
    {
      if (get_area (JKSBN_KOSUU, sizeof (struct JKT_SBN), &free_list_jksbn) < 0)
        return ((struct JKT_SBN *) -1);
      lnk_jksbn (free_list_jksbn);
    }

  rtnptr = free_jksbn_top;
  free_jksbn_top = free_jksbn_top->lnk_br;
  rtnptr->lnk_br = 0;
  rtnptr->reference = 0;
  rtnptr->status = 0;
  return (rtnptr);
}

struct JKT_SONE *
getjktsone ()
{
  register struct JKT_SONE *rtnptr;

  if (free_jksone_top == 0)
    {
      if (get_area (JKSONE_KOSUU, sizeof (struct JKT_SONE), &free_list_jksone) < 0)
        return ((struct JKT_SONE *) -1);
      lnk_jksone (free_list_jksone);
    }

  rtnptr = free_jksone_top;
  free_jksone_top = free_jksone_top->lnk_br;
  rtnptr->lnk_br = 0;
  rtnptr->jentptr = 0;
  return (rtnptr);
}
