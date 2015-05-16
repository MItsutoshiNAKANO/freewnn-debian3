/*
 * $Id: rk_multi.h,v 1.2 2001/06/14 18:16:10 ura Exp $
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
                        rk_extvars.h
                                                87.11.12  訂 補

        二つ以上のファイルにまたがって使われる変数のextern宣言。
        本体はrk_vars.cで定義。
***********************************************************************/
/*      Version 3.1     88/06/13        H.HASHIMOTO
 */
#ifdef max
#undef  max
#endif

typedef struct _modetable
{
  int max;
  int count;
  char **point;
  int size;
  char *org;
  char *next;
}
modetable;

typedef struct _modeswtable
{
  int max;
  int count;
  int *point;
}
modeswtable;

typedef struct _modenaibutable
{
  int size;
  int *org;
  int *next;
}
modenaibutable;

typedef struct _modebuftable
{
  int size;
  char *org;
}
modebuftable;

typedef struct _dat
{
  letter *code[3];
}
dat;

typedef struct _hyo
{
  int hyoshu;
  dat *data;
  letter **hensudef;
  int size;
  letter *memory;
}
hyo;

typedef struct _hyotable
{
  int size;
  hyo *point;
}
hyotable;

typedef struct _usehyotable
{
  int size;
  int *usemaehyo;
  int *usehyo;
  int *useatohyo;
}
usehyotable;

typedef struct _hyobuftable
{
  int size;
  char *org;
  char *tmp;
  char *next;
}
hyobuftable;

typedef struct _henikitable
{
  int size;
  letter *org;
  letter *next;
}
henikitable;

typedef struct _hensuset
{
  unsigned regdflg:1;           /* 既登録の変数を表す */
  unsigned curlinflg:1;         /* 現在行に既出の変数を表す */
  letter *name;
  letter *range;
}
hensuset;

typedef struct _hensuutable
{
  int max;
  int count;
  hensuset *point;
  int size;
  letter *org;
  letter *next;
}
hensuutable;

typedef struct _hensuudeftable
{
  int max;
  int count;
  letter **point;
}
hensuudeftable;

typedef struct _matchpair
{
  int hennum;
  letter ltrmch;
}
matchpair;

typedef struct _matchtable
{
  int size;
  matchpair *point;
}
matchtable;


typedef struct _RomkanTable
{
  int flags;
  jmp_buf env0;
  letter *ltrbufbgn;            /* 使用中のletterのバッファの先頭を指す */

  FILE **base;

  FILE *modefile;               /*  fp */

/* 88/06/03 V3.1 */
/* エラー処理のためのもの */
  char *hcurread;
  char *mcurdir;                /* モード表のサーチパス */
  char *mcurfnm;                /* モード表のファイル名 */
  char *mcurread;               /* モード表の現在行bufへのポインタ */

  char *tcurdir;                /* 対応表のサーチパス */
  char *tcurfnm;                /* 対応表のファイル名 */
  char *tcurread;               /* 対応表の現在行bufへのポインタ */

  char modefilename[REALFN + 5];
  char pathname[REALFN];

  modetable rk_defmode;
  modetable rk_dspmode;
  modetable rk_taiouhyo;
  modetable rk_path;

  modeswtable rk_modesw;

  modenaibutable rk_modenaibu;

  modebuftable rk_modebuf;

  hyotable rk_hyo;

  usehyotable rk_usehyo;

  hyobuftable rk_hyobuf;

  henikitable rk_heniki;

  hensuutable rk_hensuu;

  hensuudeftable rk_hensuudef;

#ifdef KDSP
#       ifdef MVUX
  letter displine[DSPLIN];      /* デバッグ用  表示の一行分を記憶 */
#       endif
#endif

    letter (*keyin_method) ();  /* キーイン関数のアドレス */
  int (*bytcnt_method) ();      /* バイトカウント関数のアドレス */
  int (*kbytcnt_method) ();     /* キー入力解釈用バイトカウント関数のアドレス */

/* モード表名（又はそのパス名）を保存 *//* 初期設定がいい加減! */
  char prv_modfnm[REALFN];
}
RomkanTable;

typedef struct _Romkan
{
  char rk_errstat;

  int flags;
  jmp_buf env0;

  char *dspmod[2][2];

/* 88/06/02 V3.1 */
  letter nil[1];
  letter *curdis;
  int codein_len;
  letter *codeout, *remainkbf;
  letter ungetc_buf;            /* romkan_ungetcが一文字退避しておくバッファ */
  letter unnext_buf;            /* romkan_unnextが             〃             */

  char eofflg;                  /* romkan_next()からLTREOFが来たときに、romkan_henkan()内で1
                                   にする。これの値が非0なら変換対応表の(error)を無視して、
                                   本処理バッファ内の未確定コードも、強制的に変換させる。更に
                                   LTREOFが来た時に特別にある出力を出すように指定されていれば
                                   その処理も行う（但し、その処理は急ごしらえで不完全）。その
                                   処理中はeofflgは2。 */

  letter evalbuf[2][2][OUTSIZ];
  letter delchr, delchr2, nisedl;       /* DELのキャラクタは二個まで持てる */

/* 88/05/31 V3.1 */
  letter rk_input;              /* 入力。3バイトまでは変更なしに対応可 */
  letter disout[OUTSIZ];        /* 本処理からの出力のバッファ */
  letter rk_output[OUTSIZ];     /* 後処理からの出力（最終出力）のバッファ */
  letter keybuf[KBFSIZ], urabuf[KBFSIZ];        /* 本処理バッファとその退避領域 */
  int lastoutlen, lastkbflen;
  int hyonum;

  char ebf_sw;
  letter oneletter[2];

  modeswtable rk_modesw;

  matchtable rk_henmatch;

  usehyotable rk_usehyo;

  RomkanTable *rk_table;
}
Romkan;

extern Romkan *cur_rk;
extern RomkanTable *cur_rk_table;
extern Romkan *romkan_init3 ();
