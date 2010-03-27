/*
 *  $Id: cvt_head.h,v 1.6 2006/03/04 19:01:45 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002, 2006
 *
 * Maintainer:  FreeWnn Project
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

/***********************************************************************
                        cvt_head.h                       ＿＿＿
                                                87/11/24｜改正｜
                                                         ￣￣￣
        convert_key.c 及び convert_read.c で使われる
        へッダファイル。テーブルのサイズの定義など。
***********************************************************************/
/*	Version 4.0	*/

#include "commonhd.h"

#if defined(luna) || defined(DGUX) || defined(hpux) || defined(sun)
#ifndef SUPPORT_TWODIGIT_FUNCTIONS
#define SUPPORT_TWODIGIT_FUNCTIONS
#endif /* SUPPORT_TWODIGIT_FUNCTIONS */
#endif /* defined(luna) || defined(DGUX) || defined(hpux) || defined(sun) */

#define BITSIZ (sizeof(int) * 8)
#define CHANGE_MAX 4            /* 変換テーブルの最大個数÷BITSIZを下回らない整数 */
#define AREASIZE 1024

#ifdef WNNDEFAULT
#  include "wnn_config.h"
 /* マクロCONVERT_FILENAMEの定義（のためだけ）。コンパイル時は、ヘッダファイル
    のサーチパスに、Wnnのインクルードファイルのありかを設定しておくこと。 */
#else
#  define CONVERT_FILENAME "cvt_key_tbl"
#endif

#define div_up(a, b) ((a + b - 1) / b)
 /* a,bは非負整数。a/bを切り上げて整数にする */

struct CONVCODE
{
  int tokey;                    /* 変換されたコード */
  char *fromkey;                /* 変換するコード */
};
