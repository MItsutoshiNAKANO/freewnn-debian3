/*
 *  $Id: error.c,v 1.16 2003/06/08 03:09:51 hiroo Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2001, 2002, 2003
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

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <stdarg.h>
#  include <string.h>
#else
#  if HAVE_MALLOC_H
#    include <malloc.h>
#  endif
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif
#endif /* STDC_HEADERS */
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
#if HAVE_SYSLOG_H
#  include <syslog.h>
#endif

#include "commonhd.h"
#include "de_header.h"
#include "jslib.h"

/* static void exit_hand (); */		/* Not used for now */
static void vwrite_log (const char *, va_list);
#ifndef HAVE_VASPRINTF
/* ansidecl.h from libiberty includes some useful macros. */
#include "ansidecl.h"

/* Have taken from libiberty.h - defined here because it isn't used anywhere */
extern int vasprintf PARAMS ((char **, const char *, va_list))
  ATTRIBUTE_PRINTF(2,0);
#endif

#if 0		/* Not used for now */
void
error_exit1 (x, y1, y2, y3, y4, y5)
     char *x;
     int y1, y2, y3, y4, y5;
{
  /* Need care with buffer size */
  char buf[512];

  strcpy (buf, "Fatal error. Exiting...: ");
  strcat (buf, x);
  log_err (buf, y1, y2, y3, y4, y5);
  exit_hand ();
}

void
error_exit (x)
     char *x;
{
  log_err("%s: %s", "Fatal error. Exiting...", x);
  exit_hand ();
}
#endif		/* Not used for now */


#if 0
/* write_log() : variable argument version of vwrite_log() */
/* not used for now */
void
write_log(const char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  vwrite_log(format, ap);
  va_end(ap);
}     
#endif

/*
   vwrite_log: 以前のmy_error()を置き換える関数。
   指定したFILE * (今のところstderr)へフォーマットをそろえて出力する。
   可変引数を使う分柔軟な使い方ができるが、移植性は損なわれるかも
   しれない。

   出力例:
   "Dec/25/2001:17:30:55 (client=hoge|server): <formatted message> ,errno=X"
   (従来の形式を踏襲しつつ(ほぼ)一行にまとめ、ログ解析をしやすくなるよう
   考慮したが、如何?)
 */
static void
vwrite_log(const char *format, va_list ap)
{
#ifdef PRINT_ERRNO
  int saved_errno = errno;
#endif
  FILE *fplog = stderr;
  time_t obakenoQ;
  struct tm *Q;
  char *chopped = NULL;
  char client_uname[WNN_ENVNAME_LEN];
  unsigned int formlen;
  const char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  obakenoQ = time(NULL);
  Q = localtime(&obakenoQ);

  fprintf(fplog, "%s/%02d/%4d:%02d:%02d:%02d ",
	  month[Q->tm_mon], Q->tm_mday, Q->tm_year + 1900,
	  Q->tm_hour, Q->tm_min, Q->tm_sec);
  if(c_c) {
    if (isgraph (c_c->user_name[0])) {
      strncpy (client_uname, c_c->user_name, WNN_ENVNAME_LEN);
      client_uname[WNN_ENVNAME_LEN-1]='\0';
    } else {
      strcpy (client_uname, "<UNKNOWN>");
    }
    fprintf(fplog, "client=%s", client_uname);
    /* getpwuid(c_c->uid)->pw_name ? */
  } else {
    fputs("server", fplog);
  }
  fputs(": ", fplog);

  formlen = strlen(format);
  if(formlen > 0  && format[formlen - 1] == '\n') {
    /* chop(format) (in Perl)-like handling -- for compatibility */
    if((chopped = (char *) malloc(formlen + 1)) != NULL) {
      strcpy(chopped, format);
      formlen--;
      while(formlen >= 0 && chopped[formlen] == '\n') {
	chopped[formlen] = '\0';
	formlen--;
      }
    }
    /* Need to add 'malloc failed' message? */
  }
      
  if(chopped != NULL) {
    vfprintf(fplog, chopped, ap);
    free(chopped);
  } else {
    vfprintf(fplog, format, ap);
  }

#ifdef PRINT_ERRNO	/* I don't know it's useful ... (aono) */
  fprintf(fplog, " ,errno=%d", saved_errno);
#endif
  fputc('\n', fplog);
  fflush(fplog);
}

void
log_debug(const char *fmt, ...)
{
  if(noisy) {
    va_list ap;

    va_start(ap, fmt);
    vwrite_log(fmt, ap);
    va_end(ap);
  }
}

void
log_err(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  if(noisy) {
    vwrite_log(fmt, ap);
  } else {
#ifdef HAVE_VSYSLOG
    vsyslog(LOG_ERR, fmt, ap);
#elif HAVE_SYSLOG
    char *tmpstr = NULL;

    vasprintf(&tmpstr, fmt, ap);
    if(tmpstr != NULL) {
      syslog(LOG_ERR, "%s", tmpstr);
      free(tmpstr);
    } else {
      syslog(LOG_ERR, "(Memory allocation failed. Cannot log messages.)");
    }
#endif /* HAVE_VSYSLOG */
  }
  va_end(ap);
}


RETSIGTYPE
signal_hand (x)
     int x;
{
  log_debug ("signal catched signal_no = %d (ignored)", x);
  re_signal (x, signal_hand);	/* See <wnn_os.h> */

  /* not reached */
#ifndef RETSIGTYPE_VOID
  return 0;
#endif
}

RETSIGTYPE
terminate_hand ()
{
  daemon_fin ();
  exit (0);

  /* not reached */
#ifndef RETSIGTYPE_VOID
  return 0;
#endif
}

#if 0		/* Not used for now */
static void
exit_hand ()
{
  daemon_fin ();
  exit (250);
}
#endif

/* FIXME: following section is not touched yet. */

/* Replace with log_debug()? */
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
