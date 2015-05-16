/*
 *  $Id: setutmp.c $
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
#include <sys/types.h>
#include <utmp.h>
#include <pwd.h>
#include "commonhd.h"
#ifdef SYSVR2
#include <memory.h>
#endif /* SYSVR2 */
#ifdef BSD42
#include <fcntl.h>
#endif /* BSD42 */

#define public

char *ttyname ();

#if defined(SVR4) && !defined(DGUX)
#include <utmpx.h>
static struct utmpx saveut;
static struct utmpx nullut;
#else /* SVR4 */
static struct utmp saveut;
static struct utmp nullut;
#endif /* SVR4 */

#ifdef BSD42
static int savslotnum = 0;
static char savttynm[8];
static int suf = 0;

#ifndef _PATH_UTMP
# ifdef UTMP_FILE
# define _PATH_UTMP UTMP_FILE
# else
#  define _PATH_UTMP "/etc/utmp"
# endif
#endif

public int
saveutmp ()
{
  register int utmpFd;
  register char *p;

  if (suf > 0)
    return 0;
  suf = -1;

  bzero (&nullut, sizeof nullut);
  if ((p = ttyname (0)) == NULL)
    return -1;
  strncpy (savttynm, rindex (p, '/') + 1, 8);
  if (!(savslotnum = ttyslot ()))
    return -1;
  if ((utmpFd = open (_PATH_UTMP, O_RDONLY, 0)) < 0)
    return -1;
  lseek (utmpFd, savslotnum * (sizeof saveut), 0);
  read (utmpFd, &saveut, sizeof saveut);
  close (utmpFd);
  strncpy (nullut.ut_line, saveut.ut_line, 8);
  strncpy (nullut.ut_host, saveut.ut_host, 16);
  nullut.ut_time = saveut.ut_time;
  suf = 1;
  return 0;
}

public int
setutmp (ttyFd)
     int ttyFd;
{
  int utmpFd;
  struct utmp ut;
  char *p;
  int i;

  if (suf <= 0)
    return -1;
  bzero (&ut, sizeof ut);
  if ((p = ttyname (ttyFd)) == NULL)
    return -1;
  strncpy (ut.ut_line, rindex (p, '/') + 1, 8);
  strncpy (ut.ut_name, getpwuid (getuid ())->pw_name, 8);
  ut.ut_time = time (0);
  strncpy (ut.ut_host, savttynm, 8);
  if (!(i = ttyfdslot (ttyFd)))
    return -1;
  if ((utmpFd = open (_PATH_UTMP, O_RDWR, 0)) < 0)
    return -1;
  lseek (utmpFd, savslotnum * (sizeof nullut), 0);
  write (utmpFd, &nullut, sizeof nullut);
  lseek (utmpFd, i * (sizeof ut), 0);
  write (utmpFd, &ut, sizeof ut);
  close (utmpFd);
  return 0;
}
#endif /* BSD42 */

#ifdef SYSVR2
public int
setutmp (ttyFd)
     int ttyFd;
{
  struct utmp ut;
  char *p;
  struct passwd *getpwuid ();

  memset (&ut, 0, sizeof ut);
  if ((p = ttyname (ttyFd)) == NULL)
    return -1;
  strncpy (ut.ut_line, strrchr (p, '/') + 1, 12);
  strncpy (ut.ut_user, getpwuid (getuid ())->pw_name, 8);
  ut.ut_time = time (0);
#ifdef DGUX
  strncpy (ut.ut_id, &ut.ut_line[3], 4);
#else
  strncpy (ut.ut_id, &ut.ut_line[2], 4);
  ut.ut_id[0] = 't';
#endif /* DGUX */
  ut.ut_pid = getpid ();
  ut.ut_type = USER_PROCESS;
  setutent ();                  /* is it necessary? */
  getutid (&ut);
  pututline (&ut);
  endutent ();
  return 0;
}
#endif /* SYSVR2 */

#ifdef BSD42
public int
resetutmp (ttyFd)
     int ttyFd;
{
  int utmpFd;
  struct utmp ut;
  char *p;
  int i;

  bzero (&ut, sizeof ut);
  if ((p = ttyname (ttyFd)) == NULL)
    return -1;
  strncpy (ut.ut_line, rindex (p, '/') + 1, 8);
  ut.ut_time = time (0);
  if (!(i = ttyfdslot (ttyFd)))
    return -1;
  if ((utmpFd = open (_PATH_UTMP, O_RDWR, 0)) < 0)
    return -1;
  lseek (utmpFd, savslotnum * (sizeof saveut), 0);
  write (utmpFd, &saveut, sizeof saveut);
  lseek (utmpFd, i * (sizeof ut), 0);
  write (utmpFd, &ut, sizeof ut);
  close (utmpFd);
  return 0;
}
#endif /* BSD42 */

#ifdef SYSVR2
public int
resetutmp (ttyFd)
     int ttyFd;
{
  struct utmp ut;
  char *p;
  struct passwd *getpwuid ();

  memset (&ut, 0, sizeof ut);
  if ((p = ttyname (ttyFd)) == NULL)
    return -1;
  strncpy (ut.ut_line, strrchr (p, '/') + 1, 12);
  strncpy (ut.ut_user, getpwuid (getuid ())->pw_name, 8);
  ut.ut_time = time (0);
#ifdef DGUX
  strncpy (ut.ut_id, &ut.ut_line[3], 4);
#else
  strncpy (ut.ut_id, &ut.ut_line[2], 4);
  ut.ut_id[0] = 't';
#endif /* DGUX */
  ut.ut_pid = getpid ();
  ut.ut_type = DEAD_PROCESS;    /* not sure */
  setutent ();                  /* is it necessary? */
  getutid (&ut);
  pututline (&ut);
  endutent ();
  return 0;
}
#endif /* SYSVR2 */
