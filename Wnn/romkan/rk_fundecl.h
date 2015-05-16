/*
 *  $Id: rk_fundecl.h,v 1.4 2005/04/10 15:26:38 aonoto Exp $
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

/***********************************************************************
                        rk_fundecl.h
                                                87.10. 9  開 業

        ライブラリ関数の型宣言集。ユーザに開放されるinclude
        ファイルは、これとrk_macros.h、及びrk_spclval.h。
***********************************************************************/
/*      Version 3.0
 */
extern letter *ltrncpy ();
extern int ltrcmp (), ltrncmp ();

extern int romkan_init (), romkan_init2 (), romkan_init3 ();
extern letter romkan_getc (), *romkan_henkan (), romkan_next ();
extern letter romkan_ungetc (), romkan_unnext ();
extern void romkan_clear ();
extern char *romkan_dispmode ();
extern char rk_errstat;         /* これは関数ではなく変数 */
