/*
 * $Id: jutil.c,v 1.2 2001/06/14 18:16:16 ura Exp $
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
/* jisho utility routine for otasuke (user interface) process */

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"

typedef struct _msg_fun
{
  char *msg;
  int (*fun) ();
}
msg_fun;

static msg_fun message1[] = {
#ifdef  USING_XJUTIL
  {"", jishoadd},
  {"", select_one_dict9},
  {"", touroku},
  {"", kensaku},
  {"", paramchg},
#endif /* USING_XJUTIL */
  {"", dicsv}
#ifdef  USING_XJUTIL
  , {"", dicinfoout},
  {"", fuzoku_set}
#endif /* USING_XJUTIL */
};

int
jutil (in)
     int in;
{
  static int last = 3;
  static int c, ret;
  static int k;
  static char *buf[sizeof (message1) / sizeof (msg_fun)];
  static WnnClientRec *c_c_sv = 0;
  static jutil_step = 0;
  static char *lang;

  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (0);
    }
  if (jutil_step == 0)
    {
      c_c_sv = c_c;
      lang = cur_lang->lang;
#ifdef  USING_XJUTIL
      for (k = 0; k < (sizeof (message1) / sizeof (msg_fun)); k++)
        {
          buf[k] = msg_get (cd, 43 + k, default_message[43 + k], lang);
        }
#else /* USING_XJUTIL */
      buf[0] = msg_get (cd, 48, default_message[48], lang);
#endif /* USING_XJUTIL */
      jutil_step++;
    }
  if (jutil_step == 1)
    {
      c = xw_select_one_element (buf, sizeof (message1) / sizeof (msg_fun), last, msg_get (cd, 20, default_message[20], lang), SENTAKU, main_table[4], in);
      if (c == BUFFER_IN_CONT)
        {
          return (BUFFER_IN_CONT);
        }
      if (c == -1 || c == -3)
        {
          c_c_sv = 0;
          jutil_step = 0;
          return (0);
        }
      jutil_step++;
      last = c;
    }
  if (jutil_step == 2)
    {
      ret = (*message1[c].fun) ();
      if (ret == BUFFER_IN_CONT)
        {
          return (BUFFER_IN_CONT);
        }
    }
  c_c_sv = 0;
  jutil_step = 0;
  return (0);
}

#ifdef  USING_XJUTIL
int
paramchg ()
{
  xw_paramchg ();
  return (0);
}
#endif /* USING_XJUTIL */

int
dicsv ()
{
  if (jl_dic_save_all (bun_data_) == -1)
    {
      errorkeyin ();
    }
  else
    {
      print_msg_getc ("%s", msg_get (cd, 0, default_message[0], cur_lang));
    }
  return (0);
}


#ifdef  USING_XJUTIL
int
dicinfoout ()
{
  xw_dicinfoout ();
  return (0);
}

int
select_one_dict9 ()
{
  xw_select_one_dict9 ();
  return (0);
}

int
fuzoku_set ()
{
  xw_fuzoku_set ();
  return (0);
}

int
jishoadd ()
{
  xw_jishoadd ();
  return (0);
}

int
kensaku ()
{
  xw_kensaku ();
  return (0);
}

#endif /* USING_XJUTIL */
