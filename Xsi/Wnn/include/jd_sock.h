/*
 *  $Id: jd_sock.h $
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

/*      jd_sock.h
        jslib header file
*/

#ifdef UX386
#undef  AF_UNIX
#include <X11/Xos.h>
#else
#include <sys/types.h>
#endif /* UX386 */
#include <sys/socket.h>
#ifdef  AF_UNIX
#include <sys/un.h>
#endif /* AF_UNIX */

#ifdef UX386
#include <net/in.h>
#else
#include <netinet/in.h>
#endif /* UX386 */

#if     defined(uniosu) || defined(UX386)
#include <net/netdb.h>          /* SX */
#else
#include <netdb.h>              /* SUN or BSD SYSV */
#endif /*uniosu */

#ifdef TAIWANESE
#ifndef CHINESE
#define CHINESE
#endif
#endif

#define ERROR   -1

#ifdef  JAPANESE                /* Japanese */
# define WNN_PORT_IN    (0x5701)
static char *sockname = "/tmp/jd_sockV4";       /* for jserver */
# define LANG_NAME      "ja_JP"
# define SERVERNAME     "wnn4"
# define MESSAGE_FILE   "jserver.msg"
#else /* JAPANESE */

#ifdef  CHINESE
#ifdef  TAIWANESE               /* Traditional Chinese */
# define WNN_PORT_IN    (0x5731)
static char *sockname = "/tmp/td_sockV4";       /* for tserver */
# define LANG_NAME      "zh_TW"
# define SERVERNAME     "wnn4_Tw"
# define MESSAGE_FILE   "tserver.msg"
#else   /* TAIWANESE */ /* Simplified Chinese */
# define WNN_PORT_IN    (0x5711)
static char *sockname = "/tmp/cd_sockV4";       /* for cserver */
# define LANG_NAME      "zh_CN"
# define SERVERNAME     "wnn4_Cn"
# define MESSAGE_FILE   "cserver.msg"
#endif /* TAIWANESE */
#else /* CHINESE */

#ifdef  KOREAN                  /* Korean */    /* not yet */
# define WNN_PORT_IN    (0x5721)
static char *sockname = "/tmp/kd_sockV4";       /* for kserver */
# define LANG_NAME      "ko_KR"
# define SERVERNAME     "wnn4_Kr"
# define MESSAGE_FILE   "kserver.msg"
#else /* KOREAN */

# define WNN_PORT_IN    (0x5701)
static char *sockname = "/tmp/jd_sockV4";       /* for jserver */
# define LANG_NAME      "ja_JP"
# define SERVERNAME     "wnn4"
# define MESSAGE_FILE   "jserver.msg"
#endif /* KOREAN */
#endif /* CHINESE */
#endif /* JAPANESE */

#define S_BUF_SIZ       1024    /* NEVER change this */
#define R_BUF_SIZ       1024    /* NEVER change this */
