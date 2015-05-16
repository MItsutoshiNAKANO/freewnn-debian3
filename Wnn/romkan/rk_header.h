/*
 *  $Id: rk_header.h,v 1.6 2005/04/10 15:26:38 aonoto Exp $
 */

/*
 * FreeWnn is a network-extensible Kana-to-Kanji conversion system.
 * This file is part of FreeWnn.
 * 
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991, 1992, 1999
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991, 1992
 * Copyright FreeWnn Project 1999, 2000, 2002
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
                        rk_header.h
                                                88. 5.20  訂 補

        rk_main.c rk_read.c rk_modread.c rk_bltinfn.cの共通ヘッダ。
        中で取っている配列の大きさなどを定義。
***********************************************************************/
/*      Version 3.0
 */
/*      make時に必要なdefine

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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef MVUX
#  define IKIS
#endif

#include <stdio.h>
#if STDC_HEADERS
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif /* STDC_HEADERS */

#include "rk_macros.h"
#include "wnn_os.h"

#define ESCCHR  '\033'
#define BASEMX  (26 + 10)

#define REALFN  200             /* 表のフルネームの最大長 */

#ifdef KOREAN
#define LINALL  30000           /* 対応表全部の行数合計 */
#define SIZALL  300000          /* 対応表の内部表現の最大サイズ */
#else
#define LINALL  2000            /* 対応表全部の行数合計 */
#define SIZALL  20000           /* 対応表の内部表現の最大サイズ・
                                   表一つの変数の変域長の合計としても使ってる */
#endif

#define LINSIZ  1000            /* 対応表の一行の最大サイズ */
#define TRMSIZ  500             /* 対応表の一項目の最大サイズ・
                                   モード名の最長としても使ってる */
#define KBFSIZ  100             /* 本処理バッファのサイズ */
#define DSPLIN  256             /* デバッグ用 */
#define OUTSIZ  200             /* 出力バッファのサイズ */
#define RSLMAX  20              /* 関数の値として返る文字列の最長 */

#define VARMAX  50              /* 表一個の変数個数 */
#define VARTOT  2000            /* 全表の変数個数計 */
#define VARLEN  500             /* 変数名の長さの計 */

#define FILNST  20

        /* rk_modread.cで使うdefine */

#define HYOMAX  40              /* 変換対応表の最大個数 */
#define HYOMEI  500             /* 表名の文字数計 */
#define PTHMAX  30              /* サーチパスの最大個数 */
#define PTHMEI  800             /* サーチパス名の文字数計 */
#define MODMAX  50              /* モードの種類数 */
#define MODMEI  300             /* モードの全文字数 */
#define DMDMAX  40              /* モード表示の種類数 */
#define DMDCHR  250             /* モード表示の全文字数 */
#define MDHMAX  2500            /* モード表の最大サイズ */
  /* モード表の最初のlistscanの時は、エラーを考慮して、リスト1個のbufferに
     表のサイズ分取っておく。 */
#define MDT1LN  200             /* モード設定リスト1個の最大長 */
#define NAIBMX  400             /* モード定義表の内部表現の最大サイズ */
                        /* Change KURI 200 --> 400 */

#define ARGMAX  2               /* 条件判断関数の引数の最大個数 */

 /* ディレクトリ名の区切りのdefine（UNIX用）。UNIX以外の環境で使うには
    これと、fixednamep()も変更の必要がある（readmode()のgetenv関係も勿論）。 */
#define KUGIRI '/'

 /* エラー処理用 */
#ifndef _WNN_SETJMP
#define _WNN_SETJMP
#include <setjmp.h>
#endif
