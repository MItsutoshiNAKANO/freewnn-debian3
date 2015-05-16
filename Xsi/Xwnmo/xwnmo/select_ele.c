/*
 * $Id: select_ele.c,v 1.4.2.1 1999/02/08 08:08:55 yamasita Exp $
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
#ifdef XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */

static int kosuu;               /* kosuu of elements */

static int
change_decimal (c1)
     int c1;
{
  if (c1 >= '0' && c1 <= '9')
    return (c1 - '0');
  if (c1 >= 'A' && c1 <= 'Z')
    return (c1 - 'A' + 10);
  if (c1 >= 'a' && c1 <= 'z')
    return (c1 - 'a' + 10);
  return (-1);
}

#define IsJIKOUHO(mode) (mode & (JIS_IN|KUTEN_IN|JIKOUHO))
#ifdef XJUTIL
#define SEL_RET (xjutil->sel_ret)
#else /* XJUTIL */
#define SEL_RET (xim->sel_ret)
#endif /* XJUTIL */
int
xw_select_one_element (c, kosuu1, init, title, mode, key_tbl, in)
     char **c;
     int kosuu1;
     int init;
     char *title;
     int mode;
     int (**key_tbl) ();
     int in;
{
  static int d;
  int c1;
  int ret;
#ifndef XJUTIL
  static WnnClientRec *c_c_sv = 0;
#endif /* !XJUTIL */
  static int select_step = 0;

#ifndef XJUTIL
  if (in == -99)
    {
      end_ichiran ();
      SEL_RET = -1;
      select_step = 0;
      ret = -1;
      goto RET;
    }
  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
#endif /* !XJUTIL */
  if (select_step == 0)
    {
      kosuu = kosuu1;
      if (init_ichiran ((unsigned char **) c, kosuu1, init, (unsigned char *) title, (unsigned char *) "", (unsigned char *) "", NULL, 0, mode) == -1)
        {
          ring_bell ();
          return (-1);
        }
#ifndef XJUTIL
      c_c_sv = c_c;
#endif /* !XJUTIL */
      select_step++;
      return (BUFFER_IN_CONT);
    }
  c1 = in;
#ifdef XJUTIL
  if (!cur_root->ichi->map)
    return (BUFFER_IN_CONT);
#else /* XJUTIL */
  if (!xim->cur_j_c_root->ichi->map)
    return (BUFFER_IN_CONT);
#endif /* XJUTIL */
  if (SEL_RET == -1)
    {
      d = change_decimal (c1);
    }
  else if (SEL_RET == -2)
    {
      end_ichiran ();
      SEL_RET = -1;
      select_step = 0;
      ret = -1;
      goto RET;
    }
  else
    {
      /*
         end_ichiran();
       */
      if (IsJIKOUHO (mode))
        {
          d = find_ji_hilite ();
        }
      else
        {
          d = find_hilite ();
        }
      SEL_RET = -1;
    }
  if (d >= 0 && d < kosuu)
    {
      end_ichiran ();
      select_step = 0;
      ret = d;
      goto RET;
    }
  if ((c1 < 256) && (key_tbl[c1] != NULL))
    {
      if ((ret = (*key_tbl[c1]) ()) == 1)
        {
          if (IsJIKOUHO (mode))
            {
              d = find_ji_hilite ();
            }
          else
            {
              d = find_hilite ();
            }
          end_ichiran ();
          select_step = 0;
          ret = d;
          goto RET;
        }
      else if (ret == -1)
        {
          end_ichiran ();
          select_step = 0;
          ret = -1;
          goto RET;
        }
      else if (ret == -3)
        {
          ring_bell ();
        }
    }
  return (BUFFER_IN_CONT);
RET:
#ifndef XJUTIL
  c_c_sv = 0;
#endif /* !XJUTIL */
  select_step = 0;
  return (ret);
}

#ifdef nodef
int
xw_select_one_jikouho (c, kosuu1, init, title, mode, key_tbl, in)
     char **c;
     int kosuu1;
     int init;
     char *title;
     int mode;
     int (**key_tbl) ();
     int in;
{
  int ret;
  static int d, c1;
  static WnnClientRec *c_c_sv = 0;
  static int select_step = 0;

  if (in == -99)
    {
      end_ichiran ();
      SEL_RET = -1;
      select_step = 0;
      ret = -1;
      goto RET;
    }
  if (c_c_sv != 0 && c_c != c_c_sv)
    {
      ring_bell ();
      return (-1);
    }
  if (select_step == 0)
    {
      kosuu = kosuu1;
      if (init_ichiran ((unsigned char **) c, kosuu1, init, (unsigned char *) title, (unsigned char *) "", (unsigned char *) "", NULL, 0, mode) == -1)
        {
          ring_bell ();
          return (-1);
        }
      c_c_sv = c_c;
      select_step++;
      return (BUFFER_IN_CONT);
    }
  else
    {
      c1 = in;
      if (!xim->cur_j_c_root->ichi->map)
        return (BUFFER_IN_CONT);
      if (xim->sel_ret == -2)
        {
          end_ichiran ();
          xim->sel_ret = -1;
          c_c_sv = 0;
          select_step = 0;
          return (-1);
        }
      else if (xim->sel_ret != -1)
        {
          d = find_ji_hilite ();
          end_ichiran ();
          xim->sel_ret = -1;
          c_c_sv = 0;
          select_step = 0;
          return (d);
        }
      else if ((c1 < 256) && (key_tbl[c1] != NULL))
        {
          if ((ret = (*key_tbl[c1]) ()) == 1)
            {
              d = find_ji_hilite ();
              end_ichiran ();
              c_c_sv = 0;
              select_step = 0;
              return (d);
            }
          else if (ret == -1)
            {
              end_ichiran ();
              c_c_sv = 0;
              select_step = 0;
              return (-1);
            }
          else if (ret == -3)
            {
              ring_bell ();
              return (BUFFER_IN_CONT);
            }
          else
            {
              return (BUFFER_IN_CONT);
            }
        }
      else
        {
          ring_bell ();
          return (BUFFER_IN_CONT);
        }
    }
}
#endif


#ifndef XJUTIL
static void
display_all ()
{
  redraw_ichi_w ();
}
#endif

int
forward_select ()
{
  xw_forward_select ();
  return (0);
}

int
backward_select ()
{
  xw_backward_select ();
  return (0);
}

int
lineend_select ()
{
  xw_lineend_select ();
  return (0);
}

int
linestart_select ()
{
  xw_linestart_select ();
  return (0);
}

int
select_select ()
{
  return (1);                   /* return mm from upper function */
}

int
quit_select ()
{
  return (-1);
}

int
previous_select ()
{
  xw_previous_select ();
  return (0);
}

int
next_select ()
{
  xw_next_select ();
  return (0);
}

#ifndef XJUTIL
int
redraw_select ()
{
  display_all ();
  return (0);
}
#endif /* !XJUTIL */

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
