/*
 *  $Id: pwd.c,v 1.6 2004/07/12 17:53:02 hiroo Exp $
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

/*
  eval(crypt(PWD, "HA")) routine.
*/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef JS
#include <stdio.h>
#if STDC_HEADERS
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif /* STDC_HEADERS */
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include "commonhd.h"
#include "jslib.h"
#include "wnn_os.h"
#endif /* !JS */

#ifdef JS
# define JS_STATIC static
#else  /* !JS */
# define JS_STATIC
#endif /* !JS */

/* etc/pwd.c */
JS_STATIC void new_pwd (char* src, char* encd);
JS_STATIC int check_pwd (char* src, char* encd);

JS_STATIC void
new_pwd (char* src, char* encd)
{
  int i, x, c;
  char xx[3];
  char *cr;

  if (encd == NULL)
    encd = src;
  if (strcmp (src, "") == 0)
    {
      bzero (encd, WNN_PASSWD_LEN);
      return;
    }
  x = time (NULL);
  xx[0] = x & 0x3f;
  xx[1] = (x & 0x3f00) >> 8;
  xx[2] = '\0';                 /* for MD5 (that requires terminator) */
  for (i = 0; i < 2; i++)
    {
      c = xx[i] + '.';
      if (c > '9')
        c += 7;
      if (c > 'Z')
        c += 6;
      xx[i] = c;
    }
  cr = crypt (src, xx);
  bzero (encd, WNN_PASSWD_LEN);
  strncpy (encd, cr, WNN_PASSWD_LEN);
}

JS_STATIC int
check_pwd (char* src, char* encd)
{
  if (strcmp (encd, "") == 0)
    return (1);                 /* No passwd */
  return (!strncmp (encd, crypt (src, encd), WNN_PASSWD_LEN));
}
