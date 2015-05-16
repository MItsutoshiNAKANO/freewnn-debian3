/*
 * $Id: rk_fundecl.h,v 1.2.2.1 1999/02/08 05:55:02 yamasita Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
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
/*      Version 3.0
 */
extern letter *ltrcpy (), *ltrncpy (), *ltrcat ();
extern int ltrlen (), ltrcmp (), ltrncmp ();

extern int romkan_init (), romkan_init2 ();
extern letter romkan_getc (), *romkan_henkan (), romkan_next ();
extern letter romkan_ungetc (), romkan_unnext ();
extern void romkan_clear ();
extern char *romkan_dispmode (), *romkan_offmode ();
extern char rk_errstat;         /* これは関数ではなく変数 */

extern RomkanTable *romkan_table_init ();
extern void rk_close ();
