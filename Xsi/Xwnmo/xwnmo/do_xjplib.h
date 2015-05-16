/*
 * $Id: do_xjplib.h,v 1.2.2.1 1999/02/08 08:08:43 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright 1991 by Massachusetts Institute of Technology
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

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

#ifdef  XJPLIB
typedef struct _XJpClientRec
{
  long dispmode;


  unsigned char c_data[128];
  Window w;
  unsigned long mask;
  long p_width, p_height;
  long x, y;
  long width, height;
  unsigned long fg, bg;
  long bp;
  long status_x, status_y;
  XIMClientRec *xim_client;
#ifdef  XJPLIB_DIRECT
  int direct_fd;
#endif                          /* XJPLIB_DIRECT */
  int ref_count;
  char escape[4][4];
  struct _XJpClientRec *next;
}
XJpClientRec;

typedef struct _XJpInputRec
{
  Window w;
  XJpClientRec *pclient;
  int save_event;
  XKeyEvent ev;
  struct _XJpInputRec *next;
}
XJpInputRec;

#define XJPLIB_PORT_IN  (0x9494)
#define UNIX_PATH       "/tmp/xwnmo.V2"
#define UNIX_ACPT       0
#define INET_ACPT       1

#define XJP_OPEN        1
#define XJP_CLOSE       2
#define XJP_BEGIN       3
#define XJP_END         4
#define XJP_VISIBLE     5
#define XJP_INVISIBLE   6
#define XJP_CHANGE      7
#define XJP_MOVE        8
#define XJP_INDICATOR   9

#define XJP_WINDOWID    1
#define XJP_ERROR       -1

#define         XJP_UNDER       0
#define         XJP_XY          1
#define         XJP_ROOT        2

#define         XJP_PWIDTH      0x00000001L
#define         XJP_PHEIGHT     0x00000002L
#define         XJP_X           0x00000004L
#define         XJP_Y           0x00000008L
#define         XJP_WIDTH       0x00000010L
#define         XJP_HEIGHT      0x00000020L
#define         XJP_FG          0x00000040L
#define         XJP_BG          0x00000080L
#define         XJP_BP          0x00000100L
#define         XJP_FONTS       0x00000200L
#define         XJP_STATUS      0x00000400L

#define XJP_F_FRONTEND_END              0
#define XJP_F_FRONTEND_ERR_END          1
#define XJP_F_JSERVER_DISCONNECT        2

#define XJP_F_OPEN_BAD_WID              3
#define XJP_F_OPEN_BAD_SZ               4
#define XJP_F_OPEN_BAD_FN               5
#define XJP_F_OPEN_BAD_DM               6
#define XJP_F_OPEN_NOT_WIN              7
#define XJP_F_OPEN_NOT_SUPPORT          8

#define XJP_F_CLOSE_BAD_CL              9

#define XJP_F_BEGIN_BAD_CL              10
#define XJP_F_BEGIN_BAD_WID             11

#define XJP_F_END_BAD_CL                12
#define XJP_F_END_BAD_WID               13

#define XJP_F_VISIBLE_BAD_CL            14

#define XJP_F_INVISIBLE_BAD_CL          15

#define XJP_F_CHANGE_BAD_CL             16
#define XJP_F_CHANGE_BAD_SZ             17
#define XJP_F_CHANGE_NOT_WIN            18
#endif
