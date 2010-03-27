/*
 *  $Id: setutmp.c,v 1.12 2009/11/28 19:20:52 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2006, 2009
 *
 * Maintainer:  FreeWnn Project
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <string.h>
#else
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#  ifdef HAVE_MEMORY_H
#    include <memory.h>
#  endif
#endif /* STDC_HEADERS */
#include <sys/types.h>
#include <pwd.h>
#if HAVE_FCNTL_H
#  include <fcntl.h>
#endif
#if TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  if HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif /* HAVE_SYS_TIME_H */
#endif /* TIME_WITH_SYS_TIME */
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#if HAVE_UTMPX_H
#  include <utmpx.h>
#endif
#if HAVE_UTMP_H
#  include <utmp.h>
#endif
#if !HAVE_UTMPX_H && !HAVE_UTMP_H
#  error "No utmp/utmpx header."
#endif

#if HAVE_STRUCT_UTMP_UT_NAME
#  define UT_USER ut_name
#elif HAVE_STRUCT_UTMP_UT_USER
#  define UT_USER ut_user
#else
#  error "No member that indicates user in struct utmp."
#endif

#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define public

#if USE_UTMP && !HAVE_LIBSPT

/* Set alias macro UTMP_TRADITIONAL */
#if !HAVE_PUTUTXLINE && !HAVE_PUTUTLINE /* && !(defined(BSD) && (BSD >= 199306)) */
#  define UTMP_TRADITIONAL 1
#endif

#ifdef UTMP_TRADITIONAL
static struct utmp saveut;
static struct utmp nullut;

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

static int
saveutmp_traditional (void)
{
  register int utmpFd;
  register char *p;

  if (suf > 0)
    return 0;
  suf = -1;

  bzero (&nullut, sizeof nullut);
  if ((p = ttyname (0)) == NULL)
    return -1;
  strncpy (savttynm, strrchr (p, '/') + 1, 8);
  if (!(savslotnum = ttyslot ()))
    return -1;
  if ((utmpFd = open (_PATH_UTMP, O_RDONLY, 0)) < 0)
    return -1;
  lseek (utmpFd, savslotnum * (sizeof saveut), 0);
  read (utmpFd, &saveut, sizeof saveut);
  close (utmpFd);
  strncpy (nullut.ut_line, saveut.ut_line, sizeof(nullut.ut_line));
  strncpy (nullut.ut_host, saveut.ut_host, sizeof(nullut.ut_host));
  nullut.ut_time = saveut.ut_time;
  suf = 1;
  return 0;
}

static int
setutmp_traditional (int ttyFd)
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

  if (!strncmp(p, "/dev/", 5))
    p += 5;
  strncpy (ut.ut_line, p, sizeof (ut.ut_line));
  strncpy (ut.UT_USER, getpwuid (getuid ())->pw_name, sizeof(ut.UT_USER));
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

static int
resetutmp_traditional (int ttyFd)
{
  int utmpFd;
  struct utmp ut;
  char *p;
  int i;

  bzero (&ut, sizeof ut);
  if ((p = ttyname (ttyFd)) == NULL)
    return -1;
  if (!strncmp(p, "/dev/", 5))
    p += 5;
  strncpy (ut.ut_line, p, sizeof(ut.ut_line));
  /* strncpy (ut.ut_line, strrchr (p, '/') + 1, 8); */
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
#endif /* UTMP_TRADITIONAL */

static void
build_utid (char *ut_id, char *ut_line, int size)
{
#ifdef DGUX
  strncpy (ut_id, &ut_line[3], size);
#else
  /* FIXME: この辺りの命名法則がよく分からない */
  strncpy (ut_id, &ut_line[2], size);
  ut_id[0] = 't';
#endif /* DGUX */
}

public int
saveutmp (void)
{
#if UTMP_TRADITIONAL && !defined(DGUX)
  return saveutmp_traditional();
#else
  return 0;
#endif
}

public int
setutmp (int ttyFd)
{
#if UTMP_TRADITIONAL
  return setutmp_traditional (ttyFd);
#else  /* !UTMP_TRADITIONAL */

#if HAVE_PUTUTXLINE
  struct utmpx utx;
  intfnptr saved_handler = NULL;
#endif
  struct utmp ut;
  int ut_err = -1;
  char *p;
  /* struct passwd *getpwuid (); */

  memset (&ut, 0, sizeof ut);
  if ((p = ttyname (ttyFd)) == NULL)
    return -1;

  if (!strncmp(p, "/dev/", 5))
    p += 5;
  strncpy (ut.ut_line, p, sizeof (ut.ut_line));
  strncpy (ut.UT_USER, getpwuid (getuid ())->pw_name, sizeof(ut.UT_USER));
  ut.ut_time = time (0);
#if HAVE_STRUCT_UTMP_UT_ID
  build_utid(ut.ut_id, ut.ut_line, 4);
  /*
   * Maybe systems that does not have struct utmp.ut_id
   * does not have utmp.ut_pid / ut_type ...
   */
  ut.ut_pid = getpid ();
  ut.ut_type = USER_PROCESS;
#endif

#if HAVE_PUTUTXLINE
  getutmpx (&ut, &utx);
# if ! HAVE_STRUCT_UTMP_UT_ID
  /* Assume all struct utmpx has this parameters ... */
  build_utid(utx.ut_id, utx.ut_line, 4);
  utx.ut_pid = getpid ();
  utx.ut_type = USER_PROCESS;
# endif
  setutxent ();                  /* is it necessary? */
  getutxid (&utx);
  /*
   * For systems that have utmp-update helper (ex. Solaris, NetBSD-4),
   * we temporally stop using chld_handler to correctly wait()
   * for helper on its implementation if user is normal user and
   * uum isn't either setuid or setgid process.
   * (You may get problem if uum catch SIGCHLD at that time ...)
   */
  if (getuid() != 0 && getuid() == geteuid() && getgid() == getegid()) {
    saved_handler = signal(SIGCHLD, SIG_DFL);
  }
  if (pututxline (&utx) == NULL) {
    /* perror("pututxline() failed:"); */	/* for DEBUG */
    ut_err = 1;
  } else {
    ut_err = 0;
  }
  if (saved_handler != NULL) {
    signal(SIGCHLD, saved_handler);	/* restore handler */
  }
  endutxent();
#endif		/* HAVE_PUTUTXLINE */
#if HAVE_PUTUTLINE
  /* Set utmp if setting utmpx fails (or non-utmpx system) */
  if (ut_err != 0) {
    setutent ();			/* is it necessary? */
    getutid (&ut);
    pututline (&ut);			/* We don't check return value */
    endutent ();
    ut_err = 0;
  }
#endif /* HAVE_PUTUTLINE */
  if (ut_err == 0) {
    return 0;
  } else {
    return -1;
  }
#endif  /* !UTMP_TRADITIONAL */
}

public int
resetutmp (int ttyFd)
{
#if UTMP_TRADITIONAL
  return resetutmp_traditional (ttyFd);
#else  /* !UTMP_TRADITIONAL */

#if HAVE_PUTUTXLINE
  struct utmpx utx;
#endif
  struct utmp ut;
  int ut_err = -1;
  char *p;
  /* struct passwd *getpwuid (); */

  memset (&ut, 0, sizeof ut);
  if ((p = ttyname (ttyFd)) == NULL)
    return -1;
  if (!strncmp(p, "/dev/", 5))
    p += 5;
  strncpy (ut.ut_line, p, sizeof (ut.ut_line));
/*   strncpy (ut.ut_line, strrchr (p, '/') + 1, 12); */
  strncpy (ut.UT_USER, getpwuid (getuid ())->pw_name, sizeof(ut.UT_USER));
  ut.ut_time = time (0);
#if HAVE_STRUCT_UTMP_UT_ID
  build_utid(ut.ut_id, ut.ut_line, 4);
  /*
   * Maybe systems that does not have struct utmp.ut_id
   * does not have utmp.ut_pid / ut_type ...
   */
  ut.ut_pid = getpid ();
  ut.ut_type = DEAD_PROCESS;    /* not sure */
#endif

#if HAVE_PUTUTXLINE
  getutmpx (&ut, &utx);
# if ! HAVE_STRUCT_UTMP_UT_ID
  /* Assume all struct utmpx has this parameters ... */
  build_utid(utx.ut_id, utx.ut_line, 4);
  utx.ut_pid = getpid ();
  utx.ut_type = DEAD_PROCESS;	/* not sure */
# endif
  setutxent ();                  /* is it necessary? */
  getutxid (&utx);
  /* We don't change SIGCHLD handler for now */
  if (pututxline (&utx) == NULL) {
    ut_err = 1;
  } else {
    ut_err = 0;
  }
  endutxent();
#endif		/* HAVE_PUTUTXLINE */
#if HAVE_PUTUTLINE
  /* Set utmp if setting utmpx fails (or non-utmpx system) */
  if (ut_err != 0) {
    setutent ();                  /* is it necessary? */
    getutid (&ut);
    pututline (&ut);		  /* We don't check return value */
    endutent ();
    ut_err = 0;
  }
#endif /* HAVE_PUTUTLINE */
  if (ut_err == 0) {
    return 0;
  } else {
    return -1;
  }
  return 0;
#endif  /* !UTMP_TRADITIONAL */
}

#else  /* ! USE_UTMP */
/* We don't use setutmp feature. Set dummy function. */
public int
saveutmp (void)
{
  return 1;
}

public int
setutmp (int ttyFd)
{
  return 1;
}

public int
resetutmp (int ttyFd)
{
  return 1;
}
#endif  /* ! USE_UTMP */
