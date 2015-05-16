/*
 *  $Id: error.c $
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

#include <config.h>

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "commonhd.h"
#include "de_header.h"
#include <pwd.h>

extern int errno;

static void my_error (), exit_hand ();
void error1 ();

void
error_exit1 (x, y1, y2, y3, y4, y5)
     char *x;
     int y1, y2, y3, y4, y5;
{
  char buf[512];

  strcpy (buf, "Fatal error. Exiting...\n");
  strcat (buf, x);
  error1 (buf, y1, y2, y3, y4, y5);
  exit_hand ();
}

void
error_exit (x)
     char *x;
{
  char buf[512];

  strcpy (buf, "Fatal error. Exiting...\n");
  strcat (buf, x);
  my_error (buf);
  exit_hand ();
}


static void
my_error (x)
     register char *x;
{
  time_t obakenoQ;
/*  struct passed *getpwiuid();  */

  if (!noisy)
    return;
  obakenoQ = time (NULL);
  if (c_c)
    {
#ifdef SYSVR2
      fprintf (stderr, "client %s :     %s      %s       errno =%d\n\n",
/*            getpwuid(c_c->uid)->pw_name, */
               c_c->user_name,  /* V3.0 */
               asctime (localtime (&obakenoQ)), x, errno);
#endif
#ifdef BSD42
      fprintf (stderr, "client %s :     %s      %s       \n\n",
/*            getpwuid(c_c->uid)->pw_name,  */
               c_c->user_name,  /* V3.0 */
               asctime (localtime (&obakenoQ)), x);
#endif
    }
  else
    {
#ifdef SYSVR2
      fprintf (stderr, "        %s      %s       errno =%d\n\n", asctime (localtime (&obakenoQ)), x, errno);

#endif
#ifdef BSD42
      fprintf (stderr, "        %s      %s       \n\n", asctime (localtime (&obakenoQ)), x);
#endif
    }
  fflush (stderr);
  errno = 0;                    /* Reset error number for next calling */
}

void
error1 (x, y1, y2, y3, y4, y5)
     register char *x;
     int y1, y2, y3, y4, y5;
{
  char buf[512];

  strcpy (buf, cmd_name);       /* strcpy(buf , "jserver:"); */
  strcat (buf, ":");
  strcat (buf, x);
  sprintf (buf, x, y1, y2, y3, y4, y5);
  my_error (buf);
}


void
signal_hand (x)
     int x;
{
  error1 ("signal catched signal_no = %d", x);
#ifdef SYSVR2
  signal (x, signal_hand);
#endif
}

void
terminate_hand ()
{
  demon_fin ();
  exit (0);
}

static void
exit_hand ()
{
  demon_fin ();
  exit (250);
}

void
out (x, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12)
     char *x;
     int y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12;
{
  if (!noisy)
    return;
  fprintf (stderr, x, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12);
  fflush (stderr);
}



#ifdef  DEBUG
/*
        debug print
*/

#ifdef  putwchar
#undef  putwchar
#endif
void
putwchar (x)
     unsigned short x;
{
  if (!noisy)
    return;
  putc (x >> 8, stderr);
  putc (x, stderr);
  /*
     putchar( x >> 8);
     putchar( x );
   */
  fflush (stdout);
}

void
wsputs (buf)
     short *buf;
{
  if (!noisy)
    return;

  for (;;)
    {
      if (*buf == 0)
        return;
      putwchar (*buf++);
    }
}
#endif /* DEBUG */
