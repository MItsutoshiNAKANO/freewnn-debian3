/*
 * $Id: w_string.c,v 1.2 2001/06/14 18:16:18 ura Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright 1991, 1992 by Massachusetts Institute of Technology
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
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#define CSWIDTH_ID      xjutil->cswidth_id
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#define CSWIDTH_ID      cur_lang->cswidth_id
#endif /* XJUTIL */
#define CONTLINE '$'

int
wchartochar (w, c)
     w_char *w;
     UCHAR *c;
{
  int flg = 0;
  w_char *w0 = w;

  for (; *w != 0; w++)
    {
      if ((*w) > 0xff)
        flg = 1;
    }
  sStrcpy (c, w0);
  return (flg);
}

int
sStrcpy (c, w)
     register UCHAR *c;
     register w_char *w;
{
  set_cswidth (CSWIDTH_ID);
  return (wnn_sStrcpy (c, w));
}

int
Sstrcpy (w, c)
     register w_char *w;
     register UCHAR *c;
{
  set_cswidth (CSWIDTH_ID);
  return (wnn_Sstrcpy (w, c));
}

w_char *
Strcat (w1, w2)
     register w_char *w1, *w2;
{
  return (wnn_Strcat (w1, w2));
}

w_char *
Strncat (w1, w2, n)
     register w_char *w1, *w2;
     register int n;
{
  return (wnn_Strncat (w1, w2, n));
}

int
Strncmp (w1, w2, n)
     register w_char *w1, *w2;
     register int n;
{
  return (wnn_Strncmp (w1, w2, n));
}

w_char *
Strcpy (w1, w2)
     register w_char *w1, *w2;
{
  return (wnn_Strcpy (w1, w2));
}

w_char *
Strncpy (w1, w2, n)
     register w_char *w1, *w2;
     register int n;
{
  return (wnn_Strncpy (w1, w2, n));
}

int
Strlen (w)
     register w_char *w;
{
  return (wnn_Strlen (w));
}

void
delete_w_ss2 (s, n)
     register w_char *s;
     register int n;
{
  wnn_delete_w_ss2 (s, n);
}
