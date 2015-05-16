/*
 *  $Id: jd_sock.h,v 1.6 2005/04/10 15:26:37 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2003
 *
 * Maintainer:  FreeWnn Project   <freewnn@tomo.gr.jp>
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

#ifndef WNN_JD_SOCK_H
#define WNN_JD_SOCK_H

/*
 * jd_sock.h
 *    jslib header file
 */

#include <sys/types.h>
#include <sys/socket.h>
#ifdef  AF_UNIX
#include <sys/un.h>
#endif /* AF_UNIX */
#include <netinet/in.h>
#include <netdb.h>

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


#endif  /* WNN_JD_SOCK_H */
