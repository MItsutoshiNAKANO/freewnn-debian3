/*
 * $Id: rk_header.h,v 1.2 2001/06/14 18:16:09 ura Exp $
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
/***********************************************************************
                        rk_header.h
                                                87.11.17  訂 補

        rk_main.c rk_read.c rk_modread.c rk_bltinfn.cの共通ヘッダ。
        中で取っている配列の大きさなどを定義。
***********************************************************************/
/*      Version 3.1     88/06/14        H.HASHIMOTO
 */
/*      make時に必要なdefine

        BSD42           BSDにてstrings.hを使用（string.hを使う場合は不要）
        SYSVR2          System Vにて定義域の制限されたtoupper・tolowerを使用
                        （なくても動く）
        MVUX            ECLIPSE MVでの運転時にdefine  IKISが自動defineされる

        RKMODPATH="文字列"
                        その文字列をモード定義表のサーチパスの環境変数の
                        名前にする
        WNNDEFAULT      「@LIBDIR」で標準設定表のあるディレクトリを表せる
                        ようにする      
        IKIS            半角仮名の１バイト目を0xA8（デフォルトは0x8E）にする

        この他 デバッグ時は必要に応じて KDSP、CHMDSPをdefine
*/

#ifdef MVUX
#       define IKIS
#endif

#include <stdio.h>
#include <X11/Xos.h>
#include "rk_macros.h"

#define fast register           /* V3.1 */

#define ESCCHR  '\033'
#define BASEMX  (26 + 10)

#define LINSIZ  512 /* 対応表の一行の最大サイズ */      /* V3.1 1000 --> 512 */
#define TRMSIZ  256 /* 対応表の一項目の最大サイズ・
                        モード名の最長としても使ってる */       /* V3.1 500 --> 256 */
#define KBFSIZ  100 /* 本処理バッファのサイズ */        /* V3.1 100 --> 128 */
#ifdef KDSP
#define DSPLIN  256             /* デバッグ用 */
#endif
#define OUTSIZ  200 /* 出力バッファのサイズ */  /* V3.1 200 --> 256 */
#define RSLMAX  10              /* 関数の値として返る文字列の最長 */

        /* rk_modread.cで使うdefine */

#define REALFN  256 /* 表のフルネームの最大長 */        /* V3.1 200 --> 256 */

#define MDT1LN  256 /* モード設定リスト1個の最大長 */   /* V3.1 200 -->256 */

#define ARGMAX  2               /* 条件判断関数の引数の最大個数 */

 /* ディレクトリ名の区切りのdefine（UNIX用）。UNIX以外の環境で使うには
    これと、fixednamep()も変更の必要がある（readmode()のgetenv関係も勿論）。 */
#define KUGIRI '/'

/* 88/06/07 V3.1 */
#define RK_DEFMODE_MAX_DEF              16      /*  */
#define RK_DEFMODE_MAX_LOT              16      /*  */
#define RK_DEFMODE_MEM_DEF              128     /*  */
#define RK_DEFMODE_MEM_LOT              128     /*  */

#define RK_DSPMODE_MAX_DEF              16      /*  */
#define RK_DSPMODE_MAX_LOT              16      /*  */
#define RK_DSPMODE_MEM_DEF              128     /*  */
#define RK_DSPMODE_MEM_LOT              128     /*  */

#define RK_PATH_MAX_DEF                 16      /*  */
#define RK_PATH_MAX_LOT                 16      /*  */
#define RK_PATH_MEM_DEF                 256     /*  */
#define RK_PATH_MEM_LOT                 256     /*  */

#define RK_TAIOUHYO_MAX_DEF             16      /*  */
#define RK_TAIOUHYO_MAX_LOT             16      /*  */
#define RK_TAIOUHYO_MEM_DEF             128     /*  */
#define RK_TAIOUHYO_MEM_LOT             128     /*  */

#define RK_HENSUU_MAX_DEF               32      /*  */
#define RK_HENSUU_MAX_LOT               32      /*  */
#define RK_HENSUU_MEM_DEF               256     /*  */
#define RK_HENSUU_MEM_LOT               256     /*  */
#define RK_HENSUUDEF_MAX_DEF            64      /*  */
#define RK_HENSUUDEF_MAX_LOT            64      /*  */

#define RK_HYO_SIZE_DEF                 4096    /*  */

 /* エラー処理用 */
#ifndef         _WNN_SETJMP
#       define  _WNN_SETJMP
#include <setjmp.h>
#endif /* _WNN_SETJMP */
