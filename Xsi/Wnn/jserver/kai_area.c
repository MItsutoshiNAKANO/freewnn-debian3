/*
 *  $Id: kai_area.c $
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

/*********************
 * kaiseki work area
**********************/

#include "commonhd.h"
#include "ddefine.h"

w_char *bun;                    /* D */

w_char giji_eisuu[20];          /* 擬似「英数」の定義 */

int maxchg;
int initjmt;                    /* I think initjmt is the length of jmt_ */

int *maxj;                      /* maxj is counts to entries in jmt_ */

/* jmt_ptr is used in jmt0.c only.
   but it must be changed by clients.
   jishobiki does not use it. this is sent to jishobiki by arguments
   */

/* j_e_p is used to hold the current point to which jmtw_ is used */
struct jdata *j_e_p;
struct jdata **jmt_;
struct jdata *jmtw_;
struct jdata **jmt_end;
struct jdata *jmtw_end;
struct jdata **jmt_ptr;
struct jdata ***jmtp;

struct FT *ft;

#ifdef  nodef
/********************************
 *      疑似自立語の品詞        *
 ********************************/
int sentou_no;                  /* 「先頭」文節先頭に成れるもの */
int suuji_no;                   /* 「数字」数詞相当 */
int katakanago_no;              /* 「カナ」外来語など 名詞相当 */
int eisuu_no;                   /* 「英数」 */
int kigou_no;                   /* 「記号」 */
int toji_kakko_no;              /* 「閉括弧」 */
int fuzokugo_no;                /* 付属語だけ */
int kai_kakko_no;               /* 「開括弧」 */
int giji_no;                    /* 疑似 */
#endif /* nodef */
