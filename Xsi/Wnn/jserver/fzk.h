/*
 *  $Id: fzk.h $
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

#define YOMI_L  8               /* 付属語の読みの長さの最大値 */

#define STRK_L  128             /* 付属語解析のための作業領域の大きさ */
                                /* これが、付属語文字列の長さの最大値 */

/* 付属語ベクトルの構造 */
struct fzkvect
{
  int no;                       /* 付属語ベクトルの番号 */
  int vector[VECT_L];           /* 付属語ベクトル */
};

/* 付属語エントリーの構造 */
struct fzkentry
{
  w_char yomi[YOMI_L];          /* 付属語の読みの文字列 */
  int yomi_su;                  /* 付属語の読みの文字列の長さ */
  int kosu;                     /* 付属語ベクトルの個数 */
  struct fzkvect *pter;         /* 付属語ベクトルへのポインタ */
  struct fzkentry *link;        /* 次の付属語エントリーへのポインタ */
};

/* 付属語の検索結果の情報の構造 */
struct fzkken
{
  struct fzkentry *ent_ptr;     /* 次の付属語エントリーへのポインタ */
  int vector[VECT_L];           /* 付属語ベクトル */
};

/* 付属語解析のための作業領域の構造 */
struct fzkwork
{
  int vector[VECT_L];           /* 付属語ベクトル */
};


/*
        structure of FZK TABLE
 */
struct FT
{
#ifndef NO_FZK
  struct fzkvect *vect_area;
#endif
  struct kangovect *kango_vect_area;
#ifndef NO_FZK
  struct fzkentry *tablefuzokugo;
#endif
  int *kango_hinsi_area;
  int fzkvect_l;                /* 接続ベクタの長さ * 32 bit */
  int kango_vect_l;             /* 幹語ベクタの長さ * 32 bit */
#ifndef NO_FZK
  int fzklength;                /* 付属語数 */
#endif
};
