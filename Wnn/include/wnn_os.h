/*
 *  $Id: wnn_os.h,v 1.17 2006/03/04 19:01:45 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2005, 2006
 *
 * Maintainer:  FreeWnn Project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef WNN_OS_H
#define WNN_OS_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* system headers needed for system dependent configuration */
#include <signal.h>
#if STDC_HEADERS
#  include <stdlib.h>
#  include <limits.h>
#endif /* STDC_HEADERS */

#include <sys/types.h>
#include <sys/file.h>
#if HAVE_SYS_PARAM_H
#  include <sys/param.h>
#endif

/* strchr vs. index, etc. */
#if (HAVE_MEMSET) && !(HAVE_BZERO)
#  define bzero(adr,n)  memset((adr),0,(n))
#endif
#if !(HAVE_STRCHR) && (HAVE_INDEX)
#  define strchr  index
#endif
#if !(HAVE_STRRCHR) && (HAVE_RINDEX)
#  define strrchr rindex
#endif

/* SIGNAL */

#ifdef SYSVR2
#ifndef re_signal
# define re_signal(x, y) signal((x), (y))
#endif
#else
# define re_signal(x, y)
#endif

#if !defined (SIGCHLD) && defined (SIGCLD)
#  define SIGCHLD SIGCLD
#endif

typedef RETSIGTYPE (*intfnptr) ();

/* Temporally place the number of filedescripters hack here. */
#if HAVE_GETDTABLESIZE
#  define WNN_NFD getdtablesize()
#elif defined (OPEN_MAX)
#  define WNN_NFD OPEN_MAX
#elif defined (NOFILE)
#  define WNN_NFD NOFILE
#endif /* HAVE_GETDTABLESIZE */

/* pseudo ramdom number */
#if !defined (RAND_MAX)
#  if defined (INT_MAX)
#    define RAND_MAX INT_MAX
#  else
#    define RAND_MAX 0x7fffffff
#  endif /* INT_MAX */
#endif /* RAND_MAX*/

#if HAVE_DRAND48
#  define DRAND()  drand48 ()
#  define SDRAND(x)  srand48 (x)
#elif HAVE_RAMDOM
#  define DRAND()  ((double) random() / (double) RAND_MAX)
#  define SDRAND(x)  srandom (x)
#else
#  define DRAND()  ((double) rand() / (double) RAND_MAX)
#  define SDRAND(x)  srand (x)
#endif /* HAVE_DRAND48 */

#if HAVE_RANDOM
#  define RAND()  random ()
#  define SRAND(x)  srandom (x)
#else
#  define RAND()  rand ()
#  define SRAND(x)  srand (x)
#endif /* HAVE_RANDOM */

/*
 * It may be needless and had better be removed, however,
 * Wnn4 did not take it for granted that the system provided
 * FD_SET and other feature that SUS v.2 determines.
 * So I left Wnn4's own definition with a little modification
 * in case the system did not provide the feature.
 * I took a look into the FreeBSD definition of FD_* family.
 * A small change comes from Canna 3.6p3.
 */
#if !defined(HAVE_FD_SET) && !defined(FD_SET) && defined(HAVE_UNISTD_H)
#  include <unistd.h> /* to define FD_SET */
#endif
#if !defined(HAVE_FD_SET) && !defined(FD_SET)
typedef unsigned long fd_mask;
#define BINTSIZE               (sizeof(unsigend long) *8)
#define SETSIZE         WNN_NFD
#define SET_WIDTH       ((SETSIZE) + (BINTSIZE - 1U) / (BINTSIZE))
typedef struct wnn_fd_set {
  wnn_fd_mask fds_bits[SET_WIDTH];
}
#define FD_SET(pos,array)  (array[pos/BINTSIZE] |= (1<<(pos%BINTSIZE)))
#define FD_CLR(pos,array)  (array[pos/BINTSIZE] &= ~(1<<(pos%BINTSIZE)))
#define FD_ISSET(pos,array)        (array[pos/BINTSIZE] &  (1<<(pos%BINTSIZ
E)))
#define FD_ZERO(array)     (bzero (array, FD_SET_WIDTH))
#endif /* !HAVE_FD_SET */


/* function prototypes (temporal use. need reconstruction) */
char *get_server_env (char *lang);	/* server_env.c */
unsigned int create_cswidth (char *s);	/* xutoj.c */
int euksc_to_ksc (unsigned char *ksc,
		  unsigned char *euksc,
		  int eusiz);		/* xutoj.c */

#endif  /* WNN_OS_H */
