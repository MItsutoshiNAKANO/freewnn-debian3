/*
 * $Id: rk_vars.c,v 1.2 2001/06/14 18:16:10 ura Exp $
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
                        rk_vars.c
                                                87.12. 3  改 正

        プログラム内で二つ以上のファイルにまたがって使う変数を
        まとめて定義している。
***********************************************************************/
/*      Version 3.1     88/06/14        H.HASHIMOTO
 */
#ifndef MULTI
#ifndef OMRON_LIB
#include "rk_header.h"
#endif

char rk_errstat = 0;            /* (error)によってエラーが引き起こされた時1（但し
                                   (eofflg)が立っていたため末尾までの強制出力処理が
                                   起こった場合は2）になる。romkan_henkan()実行ごとに
                                   更新される。これはユーザにも開放。 */

#ifdef OMRON_LIB
static
#endif
int flags = 0;
 /* 以下のフラグのOR。
    RK_CHMOUT モードチェンジを知らせるかのフラグ 
    RK_KEYACK キーインに対し必ず何か返すかのフラグ 
    RK_DSPNIL romkan_disp(off)modeの返値のデフォルトが空文字列であるかの
    フラグ。立たなければデフォルトはNULL。互換性のためromkan_
    init2まではこれが立つ。
    RK_NONISE 偽コードを出さないようにするかのフラグ
    RK_REDRAW Wnnで使う特殊なフラグ。これが立っている場合、romkan_henkan
    の結果としてdisoutにnisedl（偽物のdel）以後が特殊コードのみ
    で終わる文字列が返ってきたら、その後ろにREDRAWをつなぐ。Wnn
    で変換行のredrawをするために必要な措置。
    RK_SIMPLD deleteの動作を単純にするかのフラグ
    RK_VERBOS verboseで起こすかのフラグ
  */

#ifdef OMRON_LIB
static
#endif
jmp_buf env0;

#ifdef OMRON_LIB
static
#endif
char *dspmod[2][2] = { NULL, NULL, NULL, NULL };
         /* 現在及び一つ前の、romkanをそれぞれon・offしている時のモード表示
            文字列へのポインタ。romkan_dispmode()で返る値はdspmod[0][0]。 */


/* 88/06/07 V3.1 */
struct modetable
{
  int max;
  int count;
  char **point;
  int size;
  char *org;
  char *next;
};

#ifdef OMRON_LIB
static
#endif
struct modetable rk_defmode;
#ifdef OMRON_LIB
static
#endif
struct modetable rk_dspmode;
#ifdef OMRON_LIB
static
#endif
struct modetable rk_taiouhyo;
#ifdef OMRON_LIB
static
#endif
struct modetable rk_path;


 /* モードの状態。各々1か0の値をとる */
struct modeswtable
{
  int max;
  int count;
  int *point;
};
#ifdef OMRON_LIB
static
#endif
struct modeswtable rk_modesw;

 /* モード定義表の読み込みのためのバッファ */
struct modebuftable
{
  int size;
  char *org;
};
#ifdef OMRON_LIB
static
#endif
struct modebuftable rk_modebuf;

 /* モード定義表の内部形式のデータ */
struct modenaibutable
{
  int size;
  int *org;
  int *next;
};
#ifdef OMRON_LIB
static
#endif
struct modenaibutable rk_modenaibu;

 /* 前・本・後処理表のどれが選択されているかのデータ */
struct usehyotable
{
  int size;
  int *usemaehyo;
  int *usehyo;
  int *useatohyo;
};
#ifdef OMRON_LIB
static
#endif
struct usehyotable rk_usehyo;

struct dat                      /* 入力・出力・バッファ残り部へのポインタ */
{
  letter *code[3];
};

 /* 一つの対応表の、変換データ及び各変数の変域の、格納場所へのポインタ。 */
struct hyo
{
  int hyoshu;                   /* 表の種別(前・本・後処理表) */
  struct dat *data;             /* 変換データの開始番地 */
  letter **hensudef;            /* 変域表の開始番地 */
  int size;                     /* 格納場所のサイズ *//* V3.1 */
  letter *memory;               /* 格納場所へのポインタ *//* V3.1 */
};

struct hyotable
{
  int size;
  struct hyo *point;
};
#ifdef OMRON_LIB
static
#endif
struct hyotable rk_hyo;

struct hyobuftable
{
  int size;
  char *org;
  char *next;
};
#ifdef OMRON_LIB
static
#endif
struct hyobuftable rk_hyobuf;

struct henikitable
{
  int size;
  letter *org;
  letter *next;
};
#ifdef OMRON_LIB
static
#endif
struct henikitable rk_heniki;

struct hensuset
{
  unsigned regdflg:1;           /* 既登録の変数を表す */
  unsigned curlinflg:1;         /* 現在行に既出の変数を表す */
  letter *name;
  letter *range;
};

struct hensuutable
{
  int max;
  int count;
  struct hensuset *point;
  int size;
  letter *org;
  letter *next;
};
#ifdef OMRON_LIB
static
#endif
struct hensuutable rk_hensuu;

struct hensuudeftable
{
  int max;
  int count;
  letter **point;
};
#ifdef OMRON_LIB
static
#endif
struct hensuudeftable rk_hensuudef;

/* 現在マッチされている変数の番号と、マッチした文字の組を保持。*/
struct matchpair
{
  int hennum;
  letter ltrmch;
};
/* 変数のマッチ状況をためておく */
struct matchtable
{
  int size;
  struct matchpair *point;
};
#ifdef OMRON_LIB
static
#endif
struct matchtable rk_henmatch;
#else /*!MULTI */
#include "rk_header.h"
letter *lptr;
#endif /*!MULTI */
