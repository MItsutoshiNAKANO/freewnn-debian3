/*
 *  $Id: wnn_string.h,v 1.5 2005/04/10 15:26:37 aonoto Exp $
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

extern int wnn_sStrcpy ();
extern int wnn_Sstrcpy ();
extern void wnn_Sreverse ();
extern char *wnn_Stos ();
extern char *wnn_sStrncpy ();
extern w_char *wnn_Strcat ();
extern w_char *wnn_Strncat ();
extern int wnn_Strcmp ();
extern int wnn_Substr ();
extern int wnn_Strncmp ();
extern w_char *wnn_Strncpy ();
extern int wnn_Strlen ();
extern w_char *wnn_Strcpy ();
extern void wnn_delete_w_ss2 ();
extern int wnn_byte_count ();
#ifndef JS
extern int check_pwd ();
#endif
#ifdef CHINESE
extern int wnn_Sstrcat ();
#endif

#endif  /* WNN_STRING_H */
