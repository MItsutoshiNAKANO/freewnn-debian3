/*
 *  $Id: jlib.h,v 1.3 2001/06/14 18:15:59 ura Exp $
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

/*
        Nihongo Henkan  Library Header File
*/

#include <setjmp.h>

#define w_char  unsigned short
#ifndef WCHAR_T
#define wchar_t w_char
#define wchar   w_char
#define WCHAR_T
#endif

#define JLIB

/* header file for dic_syurui */
#define NO_EXIST        -1
#define NOT_A_JISHO     0
#define USER_DIC        1
#define SYSTEM_DIC      3

extern jmp_buf jd_server_dead_env;      /* サーバが死んだ時に飛んでいくenv */
extern int jd_server_dead_env_flg;      /* jd_server_dead_envが有効か否か  */
extern int wnn_errorno;         /* Wnnのエラーはこの変数に報告される */


struct kouho_entry
{
  int s_ichi;                   /*      かなバッファへのオフセット      */
  int jl;                       /*      自立語の長さ                    */
  int fl;                       /*      付属語の長さ                    */
  int pl;                       /*      ディスプレイ時の長さ            */
  int jishono;                  /*      自立語の辞書番号                */
  int serial;                   /*      自立語の番号                    */
  w_char *k_data;               /*      漢字バッファへのポインタ        */
};
#define KOUHO_ENT       struct  kouho_entry


struct jikouho_entry
{
  int jl;                       /*      自立語の長さ                    */
  int jishono;                  /*      自立語の辞書番号                */
  int serial;                   /*      自立語の番号                    */
  w_char *k_data;               /*      漢字バッファへのポインタ        */
};
#define JIKOUHO_ENT     struct  jikouho_entry


struct bunjoho
{
  w_char *kana_buf;             /*      かなバッファ                    */
  int kana_size;                /*      かなバッファの長さ              */
  KOUHO_ENT *klist;             /*      候補リスト・バッファ            */
  int klist_size;               /*      候補リスト・バッファの長さ      */
  w_char *kanji_buf;            /*      漢字バッファ                    */
  int kanji_buf_size;           /*      漢字バッファ の長さ             */
};


struct jikouhojoho
{
  JIKOUHO_ENT *jlist;           /*      次候補バッファ                  */
  int jlist_size;               /*      次候補バッファの長さ            */
  w_char *kanji_buf;            /*      漢字バッファ                    */
  int kanji_buf_size;           /*      漢字バッファの長さ              */
};
#define JIKOUHOJOHO     struct  jikouhojoho


struct jishojoho
{
  w_char *k_data;               /*      漢字バッファへのポインタ        */
  int bumpo;                    /*      文法情報                        */
  int hindo;                    /*      頻度                            */
  int jisho;                    /*      辞書番号                        */
  int serial;                   /*      項目番号                        */
};
#define JISHOJOHO       struct  jishojoho


struct dicinfo
{
  int dic_no;                   /*      辞書番号                        */
  int dic_size;                 /*      単語数                          */
  int ttl_hindo;                /*      頻度総数                        */
  int dic_type;                 /*      1:ユーザ,2:システム             */
  int udp;                      /*      0:使用中でない,1:使用中         */
  int prio;                     /*      辞書プライオリティ              */
  int rdonly;                   /*      0:ライト可能,1:リードオンリー   */
  char *file_name;              /*      辞書ファイル名                  */
  char *h_file_name;            /*      頻度ファイル名                  */
};
#define DICINFO         struct  dicinfo


struct jwho
{
  int sd;                       /* socket discripter in jserver */
  char *user_name;              /* user name */
  char *host_name;              /* host name */
};
#define JWHO            struct jwho
