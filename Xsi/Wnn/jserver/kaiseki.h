/*
 *  $Id: kaiseki.h $
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
 *  header of daemon 
**********************/
#define WNN_BUN         0
#define WNN_ZENKOUHO    1
#define WNN_SHO         0
#define WNN_DAI         1


#define _MININT 0x80000000
#define HIRA_HYOUKA -2000;      /* 全候補取り出しの時の平仮名の評価値 */
#define KATA_HYOUKA -1000;      /* 全候補取り出しの時の片仮名の評価値 */

#define _DIVID(val, div)        (((int)(val) > (int)0) ? ((val) / (div)) : ((val) * (div)))

extern struct fzkkouho *getfzkoh_body ();
extern struct fzkkouho *getfzkoh1_body ();

#define getfzkoh(ichbnp_p,no) \
        (((int)(no) < (int)FZKIBNO) ? &((ichbnp_p)->fzkib[no]) : getfzkoh_body(ichbnp_p,no))

#define getfzkoh1(ichbnp_p,no) \
        (((int)(no) < (int)FZKIBNO) ? &((ichbnp_p)->fzkib1[no]) : getfzkoh1_body(ichbnp_p,no))

#define kan_ck_vector(h, v) \
    (((((int)*((v) + ((h) / (sizeof(int)<<3))) >> (int)((int)(h) % (int)((int)sizeof(int)<<(int)3))) & 1) == 1) ? \
        WNN_CONNECT_BK : WNN_NOT_CONNECT_BK)

extern struct FT *ft;           /* カレントの付属語テーブル */

/* 解析エリアの管理用のリスト */
struct free_list
{
  struct free_list *lnk;
  int num;
};

#define FIRST_BZD_KOSUU 200     /* 最初に取り出す大文節解析エリア */
#define BZD_KOSUU       20      /* 足らんかったら付け加える大きさ */

#define FIRST_SBN_KOSUU 200     /* 最初に取り出す小文節解析エリア */
#define SBN_KOSUU       20      /* 足らんかったら付け加える大きさ */

#define FIRST_ICHBN_KOSUU       20      /* 最初に取り出す付属語解析エリア */
#define ICHBN_KOSUU     4       /* 足らんかったら付け加える大きさ */

#define FIRST_JKDBN_KOSUU       50      /* 最初に取り出す大文節次候補エリア */
#define JKDBN_KOSUU     10      /* 足らんかったら付け加える大きさ */

#define FIRST_JKSBN_KOSUU       50      /* 最初に取り出す小文節次候補エリア */
#define JKSBN_KOSUU     10      /* 足らんかったら付け加える大きさ */

#define FIRST_JKSONE_KOSUU      300     /* 最初に取り出す小文節次候補エリア */
#define JKSONE_KOSUU    20      /* 足らんかったら付け加える大きさ */

/************************************************
 *      擬似文節候補評価ルーチン                *
 ************************************************/
/*
#define HYOKAVAL(jiritugo_len, bnst_len) \
                hyoka1(0x80, 0, (jiritugo_len), (bnst_len), 0)
*/
/*
#define HYOKAVAL(hindo, jiritugo_len, bnst_len) \
                hyoka1(0, hindo, (jiritugo_len), (bnst_len), 0)
*/
#define HYOKAVAL(hindo, jiritugo_len, bnst_len) \
  (c_env->hindoval * (hindo) \
   + c_env->jirival * (jiritugo_len) \
   + c_env->lenval * (bnst_len) )

/************************************************
 *      小文節候補評価ルーチン                  *
 ************************************************/
#define hyoka1(HINDO, HINDO_IN, LENGTH, BUNLNGTH, PRIO) \
  (c_env->hindoval * (((HINDO) & 0x7f) + ((HINDO_IN) & 0x7f)) \
   + c_env->jirival * (LENGTH) \
   + c_env->lenval * (BUNLNGTH) \
   + c_env->jishoval * (PRIO)\
   + c_env->flagval * ((int)((HINDO) & 0x80) >> (int)7))

/************************************************
 *      大文節候補評価ルーチン                  *
 ************************************************/
/* 大文節の評価値
        sbn_val * 小文節の評価値の和 / 小文節数
           + sbn_cnt_val * 小文節数
           + dbn_len_val * 大文節長
*/
#define hyoka_dbn(SON_V, SBNCNT, DAILEN) \
        ((c_env->sbn_val * (SON_V)) / (SBNCNT) \
         + c_env->sbn_cnt_val * (SBNCNT) \
         + c_env->dbn_len_val * (DAILEN))

/* 大文節の評価値を前の文節と接続できるか否かで変える。
   divid :  1   前の文節と接続できる    hyouka * 2
            0   大文節の先頭            hyouka
           -1   前の文節と接続できない  hyouka / 2
*/
#define DIVID_HYOUKA(hyouka, divid) \
        (((divid) == 0) ? (hyouka) : \
                (((divid)) > 0 ? ((hyouka) << 1) : ((hyouka) >> 1)))

/*********************
 * kaiseki work area
**********************/
extern w_char *bun;             /* D */

extern w_char giji_eisuu[];     /* 擬似「英数」の定義 */

extern int maxchg;
extern int initjmt;             /* I think initjmt is the length of jmt_ */

extern int *maxj;               /* maxj is counts to entries in jmt_ */

/* jmt_ptr is used in jmt0.c only.
   but it must be changed by clients.
   jishobiki does not use it. this is sent to jishobiki by arguments
   */

/* j_e_p is used to hold the current point to which jmtw_ is used */
extern struct jdata *j_e_p;
extern struct jdata **jmt_;
extern struct jdata *jmtw_;
extern struct jdata **jmt_end;
extern struct jdata *jmtw_end;
extern struct jdata **jmt_ptr;
extern struct jdata ***jmtp;

/********************************
 *      疑似自立語の品詞        *
 ********************************/
#ifdef  nodef
extern int sentou_no;
extern int suuji_no;
extern int katakanago_no;
extern int eisuu_no;
extern int kigou_no;
extern int toji_kakko_no;
extern int fuzokugo_no;
extern int kai_kakko_no;
extern int giji_no;
#endif /* nodef */

#define sentou_no       0
#define suuji_no        1
#define katakanago_no   2
#define eisuu_no        3
#define kigou_no        4
#define toji_kakko_no   5
#define fuzokugo_no     6
#define kai_kakko_no    7
#define giji_no         8
