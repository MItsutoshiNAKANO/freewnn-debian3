/*
 *  $Id: rk_extvars.h $
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

/***********************************************************************
                        rk_extvars.h
                                                88. 6.16  訂 補

        二つ以上のファイルにまたがって使われる変数のextern宣言。
        本体はrk_vars.cで定義。
***********************************************************************/
/*      Version 3.0
 */
extern char rk_errstat;

extern int flags;
extern jmp_buf env0;

extern FILE *modefile;
extern char nulstr[];

extern char *hcurread, *mcurread, *curfnm, *curdir;
extern letter *ltrbufbgn;
extern FILE *nestfile[], **base;

extern struct modestat
{
  modetyp moderng;              /* モードの状態の最大値＋１ */
  modetyp curmode;              /* モードの状態 */
}
modesw[];

extern char hyoshu[];
extern char **modmeiptr, *modmeibgn[], *modmeimem, modmeimem_[];
extern char **dspnamptr, *dspnambgn[], *dspcod, dspcod_[], *dspmod[][2];

extern char **hyomeiorg, **hyomeiptr, *hyomeimem;
extern char **pathmeiorg, **pathmeiptr, *pathareaorg, *pathmeimem;

extern int usemaehyo[], usehyo[], useatohyo[], naibu_[], *naibu;

extern letter *lptr;
extern letter rk_input, disout[], rk_output[], keybuf[], urabuf[];
extern int lastoutlen, lastkbflen;
extern letter oneletter[], nil[];
extern int hyonum;

extern struct funstr            /* 組み込み関数の性質を保持するstruct */
{
  char *fnname;                 /* 関数名 */
  char appear;                  /* 入力コード部・出力コード部・バッファ残り部
                                   に書けるかどうかのフラグ３ビット */
  char argnum;                  /* 引数の個数 */
  char fntype;                  /* 結果のタイプ */
}
rk_funstr;

extern struct dat               /* 入力・出力・バッファ残り部へのポインタ */
{
  letter *code[3];
}
rk_dat;

extern struct hyo
/* 一つの対応表の、変換データ及び各変数の変域の、格納場所へのポインタ。*/
{
  struct dat *data;
  letter **hensudef;
}
rk_hyo;

extern struct funstr func[];
extern struct hyo hyo_n[];
