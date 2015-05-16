#ifndef lint
static char rcs_id[] = "$Id: killxwnmo.c,v 1.2 2001/06/14 18:16:16 ura Exp $";
#endif /* lint */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright 1992 by Massachusetts Institute of Technology
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
#include <signal.h>
#include <pwd.h>
#include "wnn_os.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "commonhd.h"
#include "sdefine.h"

void
signal_catch ()
{
  printf ("Could not get any replay from the input manager \"%s\".\n", XIM_INPUTMETHOD);
  exit (1);
}

void
main (argc, argv)
     int argc;
     char **argv;
{
  Display *dpy;
  Window window, im_id;
  XEvent event;
  Atom atom_im, prop_id = (Atom) 0;
  char uname[128], *p;
  int force = 0;
  extern int getuid ();

  if (argc > 1)
    {
      if (argv[1][0] == '-' && argv[1][1] == '9')
        force = 1;
    }
  if (!(dpy = XOpenDisplay ("")))
    {
      printf ("Could not open Display\n");
      exit (1);
    }
  if ((atom_im = XInternAtom (dpy, XIM_INPUTMETHOD, True)) == None || (im_id = XGetSelectionOwner (dpy, atom_im)) == (Window) 0)
    {
      printf ("Could not find the input manager \"%s\".\n", XIM_INPUTMETHOD);
      exit (1);
    }
  window = XCreateSimpleWindow (dpy, RootWindow (dpy, DefaultScreen (dpy)), 10, 10, 10, 10, 1, 0, 0);
  prop_id = XInternAtom (dpy, "XWNMO_KILL_PROP", False);
  if (!(p = getpwuid (getuid ())->pw_name))
    {
      printf ("Could not get the user name.\n");
      exit (1);
    }
  strcpy (uname, p);
  XChangeProperty (dpy, RootWindow (dpy, DefaultScreen (dpy)), prop_id, XA_STRING, 8, PropModeReplace, (unsigned char *) uname, strlen (uname));
  event.type = ClientMessage;
  event.xclient.format = 32;
  event.xclient.window = window;
  event.xclient.data.l[0] = XWNMO_KILL;
  event.xclient.data.l[1] = window;
  event.xclient.data.l[2] = prop_id;
  event.xclient.data.l[3] = force;
  XSendEvent (dpy, im_id, False, NoEventMask, &event);
  signal (SIGALRM, signal_catch);
  alarm (10);                   /* If return doesn't occur while 10 minutes, go to exit. */
  while (1)
    {
      XNextEvent (dpy, &event);
      if (event.type != ClientMessage || event.xclient.data.l[0] != XWNMO_KILL)
        continue;
      if (event.xclient.data.l[1] == XWNMO_KILL_OK)
        {
          printf ("Completed termination of the input manager \"%s\".\n", XIM_INPUTMETHOD);
        }
      else
        {
          printf ("Failed termination of the input manager \"%s\".\n", XIM_INPUTMETHOD);
          if (event.xclient.data.l[2] == -1)
            {
              printf ("Because permission denied.\n");
            }
          else
            {
              printf ("Because %d client(s) connect(s) to it.\n", event.xclient.data.l[2]);
            }
        }
      exit (0);
    }
}
