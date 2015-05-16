/*
 * $Id: rext.h,v 1.2.2.1 1999/02/08 05:55:00 yamasita Exp $
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

/*
 * rk_main.c
 */
extern char *chrcat ();
extern letter *ltrgrow ();
extern int ltrlen ();
extern char *strend ();
extern int ltrcmp ();
extern letter *ltr1cut ();
extern int ltrstrcmp ();
extern void BUGreport ();

/*
 * rk_alloc.c
 */
extern void malloc_for_modetable_struct ();
extern void free_for_modetable_struct ();
extern void check_and_realloc_for_modetable_struct ();
extern void malloc_for_modesw ();
extern void free_for_modesw ();
extern void check_and_realloc_for_modesw ();
extern void malloc_for_modebuf ();
extern void free_for_modebuf ();
extern void malloc_for_modenaibu ();
extern void realloc_for_modenaibu ();
extern void free_for_modenaibu ();
extern void malloc_for_hyo ();
extern void free_for_hyo ();
extern void malloc_for_usehyo ();
extern void free_for_usehyo ();
extern void malloc_for_hyo_area ();
extern void free_for_hyo_area ();
extern void malloc_for_hensuudef ();
extern void free_for_hensuudef ();
extern void check_and_realloc_for_hensuudef ();
extern void malloc_for_hensuu ();
extern void free_for_hensuu ();
extern void check_and_realloc_for_hensuu ();
extern void malloc_for_henmatch ();
extern void free_for_henmatch ();
extern void malloc_for_hyobuf ();
extern void free_for_hyobuf ();
extern void check_and_realloc_for_hyobuf ();
extern void malloc_for_heniki ();
extern void free_for_heniki ();
extern void check_and_realloc_for_heniki ();

/*
 * rk_bltinfn.c
 */
extern letter to_zenalpha ();
extern void to_hankata ();
extern letter to_zenhira ();
extern letter to_zenkata ();
void handakuadd ();
void dakuadd ();
void to_digit ();

/*
 * rk_modread.c
 */
int readmode ();
int readfnm ();
void choosehyo ();
int filnamchk ();

/*
 * rk_read.c
 */
extern void readdata ();
extern void ltr1tostr ();
extern letter get1ltr ();
extern letter unget1ltr ();
extern int int_get1ltr ();
extern int int_unget1ltr ();
extern letter getfrom_dblq ();
extern int getfrom_lptr ();
extern void de_bcksla ();
extern int ltov ();
extern letter vtol ();
