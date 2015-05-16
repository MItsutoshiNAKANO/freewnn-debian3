/*
 * $Id: history.c,v 1.3.2.1 1999/02/08 08:08:46 yamasita Exp $
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
#include "jslib.h"
#include "ext.h"

int
init_history ()
{
  int size1;
  char *area_pter;

  register History *hist, *pre, *end;

  if ((history_cunt = max_history) < 1)
    {
      history_cunt = 0;
      return (0);
    }
  size1 = history_cunt * sizeof (History);
  if ((area_pter = Malloc ((unsigned) size1)) == NULL)
    {
      history_cunt = 0;
      wnn_errorno = WNN_MALLOC_ERR;
      return (-1);
    }
  hist = beginning_of_history = end_of_history = add_history = (History *) area_pter;
  c_c->save_hist = (History *) area_pter;
  pre = end = hist + (history_cunt - 1);
  do
    {
      hist->hbuffer = NULL;
      pre->next = hist;
      hist->previous = pre;
      hist->num = 0;
      pre = hist++;
    }
  while (hist <= end);
  return (0);
}

static void
set_up_history ()
{
  if (add_history == beginning_of_history && end_of_history != beginning_of_history)
    beginning_of_history = beginning_of_history->next;
  end_of_history = add_history;
  add_history = add_history->next;
  current_history = NULL;
}

int
make_history (wstr, n)
     register w_char *wstr;
     register int n;
{
  register char *p;

  if (!history_cunt)
    return (0);
  if (n <= 0 || n > maxchg || *wstr == 0)
    return (-1);

  if (n == 1 && NORMAL_CHAR (*wstr))
    {
      if (!wchar_holding)
        {
          wchar_holding = 1;
          set_up_history ();
          end_of_history->num = 0;
        }
      if (!(p = Malloc ((unsigned) ((end_of_history->num + 1) * sizeof (w_char)))))
        {
          malloc_error ("allocation of data for history");
          return (-1);
        }
      if (end_of_history->hbuffer)
        Free (end_of_history->hbuffer);
      end_of_history->hbuffer = (w_char *) p;
      Strncpy (end_of_history->hbuffer + end_of_history->num, wstr, 1);
      if (++(end_of_history->num) >= maxchg)
        wchar_holding = 0;
      return (0);
    }
  if (wchar_holding)
    wchar_holding = 0;
  if (n == 1 && !(KANJI_CHAR (*wstr)))
    /* don't insert cntrol code in history buffer */
    return (0);
  if (end_of_history->num == n && Strncmp (end_of_history->hbuffer, wstr, n) == 0)
    {
      current_history = NULL;
      return (0);
    }
  if (!(p = Malloc ((unsigned) (n * sizeof (w_char)))))
    {
      malloc_error ("allocation of data for history");
      return (-1);
    }
  set_up_history ();
  if (end_of_history->hbuffer != NULL)
    Free ((char *) end_of_history->hbuffer);
  end_of_history->hbuffer = (w_char *) p;
  Strncpy (end_of_history->hbuffer, wstr, n);
  end_of_history->num = n;

  return (0);
}

static int
get_current_history (wbuf)
     register w_char *wbuf;
{
  if (!history_cunt)
    return (0);
  if (wchar_holding)
    wchar_holding = 0;
  if (current_history == NULL)
    current_history = end_of_history;
  Strncpy (wbuf, current_history->hbuffer, current_history->num);
  return (current_history->num);
}

void
get_end_of_history (wbuf)
     register w_char *wbuf;
{
  if (!history_cunt)
    {
      *wbuf = 0;
    }
  else
    {
      if (wchar_holding)
        wchar_holding = 0;
      Strncpy (wbuf, end_of_history->hbuffer, end_of_history->num);
      *(wbuf + end_of_history->num) = 0;
    }
}

int
previous_history1 (buffer)
     register w_char *buffer;
{
  if (!history_cunt || current_history == beginning_of_history)
    return (0);                 /* do nothing */

  if (current_history != NULL)
    current_history = current_history->previous;
  return (get_current_history (buffer));
}

int
next_history1 (buffer)
     register w_char *buffer;
{
  if (!history_cunt || current_history == end_of_history || current_history == NULL)
    return (0);

  current_history = current_history->next;
  return (get_current_history (buffer));
}

void
destroy_history ()
{
  register History *p;
  register int i;

  if (c_c == NULL || c_c->save_hist == NULL)
    return;
  for (i = 0, p = c_c->save_hist; i < history_cunt; i++, p++)
    {
      if (p->hbuffer)
        Free ((char *) p->hbuffer);
    }
  Free ((char *) c_c->save_hist);
}
