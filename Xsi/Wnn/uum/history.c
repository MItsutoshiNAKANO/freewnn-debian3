/*
 *  $Id: history.c $
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
#include "sdefine.h"
#include "sheader.h"
#include "jslib.h"

struct history
{
  w_char *hbuffer;
  struct history *previous;
  struct history *next;
  int num;
};

static struct history *beginning_of_history;    /* ヒストリの先頭 */
static struct history *end_of_history;  /* ヒストリの最後 */
static struct history *add_history;     /* 追加位置 */
static struct history *current_history = NULL;  /* index */
static int wchar_holding = 0;
static int history_cunt;

static void set_up_history ();

int
init_history ()
{
  char *malloc ();
  int size1;
  char *area_pter;

  register struct history *hist, *pre, *end;

  if ((history_cunt = max_history) < 1)
    {
      history_cunt = 0;
      return (0);
    }
  size1 = history_cunt * sizeof (struct history);
  if ((area_pter = malloc (size1)) == NULL)
    {
      history_cunt = 0;
      wnn_errorno = WNN_MALLOC_ERR;
      return (-1);
    }
  hist = beginning_of_history = end_of_history = add_history = (struct history *) area_pter;
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


int
make_history (wstr, n)
     register w_char *wstr;
     register int n;
{
  extern char *malloc ();

  if (!history_cunt)
    return (0);
  if (n <= 0 || n > maxchg || *wstr == NULL)
    return (-1);

  if (n == 1 && NORMAL_CHAR (*wstr))
    {
      if (!wchar_holding)
        {
          wchar_holding = 1;
          set_up_history ();
          end_of_history->num = 0;
        }
      if (end_of_history->hbuffer == NULL)
        end_of_history->hbuffer = (w_char *) malloc (sizeof (w_char));
      else
        end_of_history->hbuffer = (w_char *) realloc (end_of_history->hbuffer, (end_of_history->num + 1) * sizeof (w_char));
      Strncpy (end_of_history->hbuffer + end_of_history->num, wstr, 1);
      if (++(end_of_history->num) >= maxchg)
        wchar_holding = 0;
      return (0);
    }
  if (wchar_holding)
    wchar_holding = 0;
  if (n == 1 && !(KANJI_CHAR (*wstr)))
    /* コントロールコードは、ヒストリには入れない */
    return (0);
  if (end_of_history->num == n && Strncmp (end_of_history->hbuffer, wstr, n) == 0)
    {
      /* 同じものは、いや */
      current_history = NULL;
      return (0);
    }
  set_up_history ();
  if (end_of_history->hbuffer != NULL)
    free (end_of_history->hbuffer);
  end_of_history->hbuffer = (w_char *) malloc (n * sizeof (w_char));
  Strncpy (end_of_history->hbuffer, wstr, n);
  end_of_history->num = n;

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


/* 確定後に登録する漢字を取り出す */
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
