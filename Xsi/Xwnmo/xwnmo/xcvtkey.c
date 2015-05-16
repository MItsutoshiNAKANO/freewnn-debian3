/*
 * $Id: xcvtkey.c,v 1.7.2.1 1999/02/08 08:09:00 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
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

/*************************************************************************
                xcvtkey.c
*/

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif /* XJUTIL */

char *comment_char_str = "#;";

int
comment_char (c)
     register char c;
{
  return ((index (comment_char_str, c) != NULL) ? 1 : 0);
}

typedef struct
{
  char *name;
  unsigned int state;
}
StateTbl;

static StateTbl state_tbl[] = {
  {"Shift", ShiftMask},
  {"Lock", LockMask},
  {"Ctrl", ControlMask},
  {"Meta", Mod1Mask},
  {"Meta1", Mod1Mask},
  {"Meta2", Mod2Mask},
  {"Meta3", Mod3Mask},
  {"Meta4", Mod4Mask},
  {"Meta5", Mod5Mask},
  {NULL, 0}
};

static unsigned int
get_one_entry (p)
     char **p;
{
  register char *s;
  register int size;
  register StateTbl *st = state_tbl;
  char buf[512];

  if (!*p || !**p)
    return (-1);
  if (s = index (*p, '|'))
    {
      size = s - *p;
      bcopy (*p, buf, size);
      buf[size] = '\0';
      for (; st->name; st++)
        {
          if (!strcmp (st->name, buf))
            {
              *p += size + 1;
              return (st->state);
            }
        }
    }
  return (0);
}

int
cvt_key_setup (filename)
     char *filename;
{
  register int k, cnt = cvt_key_tbl_cnt, line = 0;
  register unsigned int state, total_state;
  KeySym c;
  char ksname[128], code[32], *p;
  char buf[BUFSIZ];
  FILE *fp;
  register ConvCode *tbl = cvt_key_tbl;
  extern long strtol ();

  if ((fp = fopen (filename, "r")) == NULL)
    {
      return (-1);
    }

CONTI:
  while (fgets (buf, BUFSIZ, fp))
    {
      line++;
      if (comment_char (*buf) || (k = sscanf (buf, "%s %s", ksname, code)) <= 0)
        continue;
      if (k < 2)
        {
          print_out2 ("In cvt file \"%s\", too few arguments in line %d.", filename, line);
        }
      else
        {
          p = ksname;
          total_state = 0;
          while (1)
            {
              state = get_one_entry (&p);
              if (state <= 0)
                break;
              total_state |= state;
            }
          if (state == -1 || (c = XStringToKeysym (p)) == 0)
            {
              print_out2 ("In cvt file \"%s\", a illegal string in line %d.", ksname, line);
              continue;
            }
          for (k = 0; k < cnt; k++)
            {
              if (tbl[k].fromkey.keysym == c && tbl[k].fromkey.state == total_state)
                {
                  print_out2 ("In cvt file \"%s\", duplicate declaration in line %d.", ksname, line);
                  goto CONTI;
                }
            }
          tbl[cnt].fromkey.keysym = c;
          tbl[cnt].fromkey.state = total_state;
          tbl[cnt++].tokey = strtol (code, (char **) NULL, 0);
        }
      if (cnt >= MAX_CVT_ENTRY_CNT)
        {
          print_out2 ("In cvt file \"%s\", max line is %d.", ksname, MAX_CVT_ENTRY_CNT);
          break;
        }
    }
  fclose (fp);
  cvt_key_tbl_cnt = cnt;
  return (cnt);
}


/* Function of convert */

int
cvt_key_fun (keysym, state)
     KeySym keysym;
     int state;
{
  register int cnt;
  register ConvCode *tbl = cvt_key_tbl;

  for (cnt = 0; cnt < cvt_key_tbl_cnt; cnt++)
    {
      if (tbl[cnt].fromkey.keysym == keysym && tbl[cnt].fromkey.state == state)
        return (tbl[cnt].tokey);
    }
  return (-1);
}

/* Backward compatibility for cvt_meta and cvt_fun. */
int
cvt_meta_and_fun_setup (filename, tbl, state_name)
     char *filename;
     ConvCode *tbl;
     char *state_name;
{
  register int k, cnt = cvt_key_tbl_cnt, line = 0;
  register StateTbl *st = state_tbl;
  KeySym c;
  unsigned int state = 0;
  char ksname[32], code[32];
  char buf[BUFSIZ];
  FILE *fp;
  extern long strtol ();

  if ((fp = fopen (filename, "r")) == NULL)
    {
      return (-1);
    }

  if (state_name && *state_name)
    {
      for (; st->name; st++)
        {
          if (!strcmp (st->name, state_name))
            {
              state = st->state;
              break;
            }
        }
    }
  while (fgets (buf, BUFSIZ, fp))
    {
      line++;
      if (comment_char (*buf) || (k = sscanf (buf, "%s %s", ksname, code)) <= 0)
        continue;
      if (k < 2)
        {
          print_out2 ("In cvt file \"%s\", too few arguments in line %d.", filename, line);
        }
      else if ((c = XStringToKeysym (ksname)) == 0)
        {
          print_out2 ("In cvt file \"%s\", a illegal Keysym in line %d.", ksname, line);
        }
      else
        {
          tbl[cnt].fromkey.keysym = c;
          tbl[cnt].fromkey.state = state;
          tbl[cnt].tokey = strtol (code, (char **) NULL, 0);
          cnt++;
        }
    }
  fclose (fp);
  return (cnt);
}
