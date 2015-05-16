/*
 * $Id: rk_spclval.h,v 1.2 2001/06/14 18:16:10 ura Exp $
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
/*      Version 3.0
 */
#ifndef EOLTTR

typedef unsigned int letter;    /* 文字は一般にこの型で表す */
typedef unsigned char uns_chr;

typedef unsigned char modetyp;
#define LTRHUG  (~(0xff000000))
        /* 内部表現 及び偽文字を除く、letterの取り得る最大値 */


        /* 以下に定義する値は、最上位バイトが255でなければならない。
           これらのうち、外に出るのは EOLTTR、CHMSIG、NISEBP、LTREOFと
           REDRAW（偽デリートとしてNISEDLを使う場合はNISEDLも）。 */

#define EOLTTR  (0xffffffff)    /* 文字列の終端を表すコード */
                        /* キー入力があったら必ず何かコードを返す状態にして
                           おくと、特に返すべきものがない時はこれを返す。 */
#define ERRCOD  (EOLTTR-1)      /* (error)を表す内部コード */
#define CHMSIG  (EOLTTR-2)      /* モードチェンジを表すコード */
#define NISEBP  (EOLTTR-3)      /* エラー警告用の偽BELコード */
#define VARRNG  (EOLTTR-4)      /* 変域が二つの文字の間であるのを示す内部コード */
#define UNUSDC  (EOLTTR-5)      /* マッチを失敗させるための内部コード */
#define REASIG  (EOLTTR-6)      /* 表の再設定を要求するための内部コード */
#define URBFCL  (EOLTTR-7)      /* 裏バッファの明示的クリアを要求する内部コード */
#define LTREOF  (EOLTTR-8)      /* romkan_next()、同getc()がEOFの代わりに返すもの
                                   （できればEOFと同じにしたいが…） */
#define REDRAW  (EOLTTR-9)      /* Wnnに変換行のredrawを要求する特殊コード */
#define NISEDL  (EOLTTR-10)     /* 偽デリートとして定数を使いたい人のために準備
                                   された定数（但し、使いたくなければ使わなくても
                                   いい）。変数nisedlにセットして使う */
#define SENDCH  (EOLTTR-11)     /* 変換バッファが空なら、次の文字を無条件に
                                   上に送るようWnnに要求するエスケープコード。 */


        /* romkan_init3()の引数のフラグに使う値 */

#define RK_CHMOUT 01            /* モードチェンジを知らせるコードを返すか？ */
#define RK_KEYACK 02            /* キーインに対し必ず何かを返すか */
#define RK_DSPNIL 04            /* モード表示文字列無指定の時に空文字列を返すか（デフ
                                   ォルトはNULL）。互換性保持のため */
#define RK_NONISE 010           /* 偽コードを出さないようにするか */
#define RK_REDRAW 020           /* Wnn用特殊フラグ（redraw用のフラグを出すかどうか） */
#define RK_SIMPLD 040           /* deleteの動作を単純にするか */
#define RK_VERBOS 0100          /* 使用する表の一覧をリポートするか */


        /* コードの区別に使うマクロ */

#define HG1BIT  (0x80000000)    /* 最上位ビットだよ */
#define SHUBET(X) ((letter)(X) >> 24)
                              /* 内部表現で、上１バイトを種別表現に使ってる */
#define LWRMSK(X) ((X) & ~(0xff000000)) /* 上１バイトを取り除く */
#define LWRCUT(X) ((X) &= ~(0xff000000))        /* 上１バイトを取り除く */

#define is_HON(X) (SHUBET(X) == 0)      /* 本物の文字か */
#define NAIBEX(X) ((int)0 < SHUBET(X) && (int)SHUBET(X) < 0x80) /* 内部表現を示す値か */
#define isNISE(X) (SHUBET(X) == 0x80)   /* 偽物の文字か（最上位ビットが立つ） */
#define isSPCL(X) (SHUBET(X) == 0xff)   /* rk_spcl_val.hで定義される値かどうか */
 /* NISEDLなどを含めた偽の文字であるかどうかを判定するには、~is_HON(X) か、
    isNISE(X) || isSPCL(X) として判定しないといけない。 */

#define toNISE(X) ((X) | HG1BIT)
#define to_HON(X) ((X) & ~HG1BIT)

 /* 互換性のため用意してある別名 */
#define REALCD(X) is_HON(X)
#define NISECD(X) isNISE(X)
#define SPCLCD(X) isSPCL(X)

#ifndef SS2
#define SS2     0x8E
#endif
#ifndef SS3
#define SS3     0x8F
#endif

        /** rk_bltinfn.c の補完のためのマクロ
            （引数を複数回評価するものも多いので注意）*/

#define HKKBGN  (SS2 * 0x100 + 0xA1)    /* 半角カナの句点 */
#define HKKEND  (SS2 * 0x100 + 0xDF)    /*     〃    半濁点 */
#define HIRBGN  (0xA4A1)        /* ぁ */
#define HIREND  (0xA4F3) /* ん */       /* ひらがな："ぁ"〜"ん" */
#define KATBGN  (0xA5A1)        /* ァ */
#define KATEND  (0xA5F6) /* ヶ */       /* カタカナ："ァ"〜"ン"〜"ヶ" */

#define _to_kata(l) ((l) + (KATBGN - HIRBGN)) /** カタカナへ（定義域制限）*/
#define _to_hira(l) ((l) - (KATBGN - HIRBGN)) /** ひらがなへ（定義域制限）*/
#define is_hira(l) ((int)HIRBGN <= (int)(l) && (int)(l) <= (int)HIREND) /** ひらがなか？ */
#define is_kata(l) ((int)KATBGN <= (int)(l) && (int)(l) <= (int)KATEND) /** カタカナか？ */
#define is_kata2(l) ((int)_to_kata(HIRBGN) <= (int)(l) && (int)(l) <= (int)_to_kata(HIREND))
                                /** 対応するひらがなのあるカタカナか？ */
#define to_kata(l) (is_hira(l) ? _to_kata(l) : (l)) /** カタカナへ */
#define to_hira(l) (is_kata2(l) ? _to_hira(l) : (l))
                        /** ひらがなへ。「ヴヵヶ」はカタカナのまま残る。*/
#define is_hankata(l) ((int)HKKBGN <= (int)(l) && (int)(l) <= (int)HKKEND)
                                /** 半角カナ（句点などを含む）か？ */



        /* その他のマクロ関数群（引数を複数回評価するものも多いので注意） */

#define numberof(array) (sizeof(array) / sizeof(*array))

 /* ポインタをletterの列の末尾へもっていく。letter *lp */
#define totail(lp) {while(*(lp) != EOLTTR) (lp)++;}

 /* 文字列へのポインタをその文字列の最後尾へ。totailのchar版。char *sp */
#define strtail(sp) {while(*(sp)) (sp)++;}

        /* 限定版romkan_init3 */
#define romkan_init4(delchr, nisedl, flg) \
        romkan_init3(delchr, nisedl, EOLTTR, flg)

#define romkan_init5(delchr, flg) \
        romkan_init4(delchr, toNISE(delchr), flg)


#endif /* of ifndef EOLTTR */
