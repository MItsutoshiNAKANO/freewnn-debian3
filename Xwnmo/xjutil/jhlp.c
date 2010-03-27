#ifndef lint
static char rcs_id[] = "$Id: jhlp.c,v 1.2 2001/06/14 18:16:12 ura Exp $";
#endif /* lint */

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
#include <signal.h>
#include <pwd.h>
#include "commonhd.h"
#include "sdefine.h"
#include <X11/Xos.h>
#include "xjutil.h"
#include "sxheader.h"
#include "config.h"
#include "xext.h"

struct passwd *getpwuid ();

#ifdef  BSD42
#include <sgtty.h>
#endif /* BSD42 */
#ifdef SYSVR2
#include <termio.h>
#endif /* SYSVR2 */

extern char *optarg;
extern int optind;

static void
do_end ()
{
  xw_end ();
  disconnect_server ();
  exit (0);
}

void
terminate_handler ()
{
  do_end ();
}

static void
do_main ()
{
  int buf[2];

  for (;;)
    {
      xw_read (buf);
    }
}

static int
keyin0 ()
{
  static int buf[BUFSIZ];
  static int *bufend = buf;
  static int *bufstart = buf;
  int n;

  if (bufstart < bufend)
    {
      return (*bufstart++);
    }
  for (;;)
    {
      if ((n = xw_read (buf)) > 0)
        {
          bufstart = buf;
          bufend = buf + n;
          return (*bufstart++);
        }
    }
}

static int
conv_keyin ()
{
  int keyin0 ();

  return (keyin0 ());
}

int
keyin ()
{
  return (conv_keyin ());
}


/*
  signal settings
 */

static void
save_signals ()
{
  signal (SIGPIPE, SIG_IGN);
  signal (SIGHUP, SIG_IGN);
  signal (SIGQUIT, SIG_IGN);
  signal (SIGTERM, terminate_handler);
  signal (SIGCHLD, SIG_IGN);
}

void
main (argc, argv)
     int argc;
     char **argv;
{
  extern char xjutil_name[];
  extern int counter;
  prgname = argv[0];

#ifdef  DEBUG
  fprintf (stderr, "%s, %s, %s, %d\n", argv[0], argv[1], argv[2], argv[3][0]);
#endif /* DEBUG */
  display_name = (char *) alloc_and_copy (argv[1]);
  strcpy (xjutil_name, argv[2]);
  counter = (int) argv[3][0];

  save_signals ();

  maxchg = MAXCHG;
  maxlength = MAXCHG * 2;
  maxbunsetsu = MAXBUNSETSU;

  if (alloc_all_buf () == -1)
    exit (1);

  if (create_xjutil () == -1)
    {
      exit (-1);
    }
  if (init_xcvtkey () == -1)
    {
      return_error ();
      exit (-1);
    }

  switch (init_wnn ())
    {
    case -1:
      terminate_handler ();
      break;
    case -2:
      disconnect_server ();
      do_end ();
      break;
    }
  connect_server ();

  do_main ();
}
