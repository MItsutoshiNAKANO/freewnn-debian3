/*
 *  $Id: wnn_string.h,v 1.6 2013/09/02 11:01:39 itisango Exp $
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

#ifndef WNN_STRING_H
#define WNN_STRING_H

#include "jslib.h"

extern int wnn_sStrcpy FRWNN_PARAMS((register char*, register w_char*));
extern int wnn_Sstrcpy FRWNN_PARAMS((w_char*,unsigned char*));
extern void wnn_Sreverse FRWNN_PARAMS((w_char*,w_char*));
extern char *wnn_Stos FRWNN_PARAMS((w_char*));
extern char *wnn_sStrncpy FRWNN_PARAMS((register char*,register w_char*,int));
extern w_char *wnn_Strcat FRWNN_PARAMS((register w_char*,register w_char*));
extern w_char *wnn_Strncat FRWNN_PARAMS((register w_char*,register w_char*,int));
extern int wnn_Strcmp FRWNN_PARAMS((register w_char*,register w_char*));
extern int wnn_Substr FRWNN_PARAMS((register w_char*,register w_char*));
extern int wnn_Strncmp FRWNN_PARAMS((register w_char*,register w_char*,int));
extern w_char *wnn_Strncpy FRWNN_PARAMS((register w_char*,register w_char*,int));
extern int wnn_Strlen FRWNN_PARAMS((register w_char*));
extern w_char *wnn_Strcpy FRWNN_PARAMS((register w_char*,register w_char*));
extern void wnn_delete_w_ss2 FRWNN_PARAMS((register w_char*,int));
extern int wnn_byte_count FRWNN_PARAMS((register int));
#ifndef JS
extern int check_pwd FRWNN_PARAMS((char*,char*));
#endif
#ifdef CHINESE
extern int wnn_Sstrcat FRWNN_PARAMS((w_char*,unsigned char*));
#endif

#endif  /* WNN_STRING_H */
