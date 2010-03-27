/*
 *  $Id: rk_spclval.h,v 1.4 2005/04/10 15:26:38 aonoto Exp $
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
                        rk_spclval.h
                                                88. 6. 9  訂 補

        本変換で使っている特殊な値の定義（外部にも出す）。マクロ
        関数も、ctype.hを使うもの以外はこちらで定義されている。
        rk_macros.h にincludeされている。
***********************************************************************/
/*      Version 3.0
 */
#ifndef EOLTTR

typedef unsigned int letter;    /* 文字は一般にこの型で表す */
typedef unsigned char uns_chr;

typedef unsigned char modetyp;
#define ModeNotInt              /* 上でmodetypがunsigned intより小さい型にtypedefされて
                                   いたらこれをdefineして下さい。 */
/* #define ModeHasBuf */

#define LTRHUG  (~(0xff000000))
        /* 内部表現 及び偽文字を除く、letterの取り得る最大値 */


        /* 以下に定義する値は、最上位バイトが255でなければならない。
           これらのうち、外に出るのは EOLTTR、CHMSIG、NISEBP、LTREOF、REDRAWと
           SENDCH（偽デリートとしてNISEDLを使う場合はNISEDLも）。 */

#define EOLTTR  ((letter)-1)    /* 文字列の終端を表すコード */
                        /* キー入力があったら必ず何かコードを返す状態にして
                           おくと、特に返すべきものがない時はこれを返す。 */
#define ERRCOD  ((letter)-2)    /* (error)を表す内部コード */
#define CHMSIG  ((letter)-3)    /* モードチェンジを表すコード */
#define NISEBP  ((letter)-4)    /* エラー警告用の偽BELコード */
#define VARRNG  ((letter)-5)    /* 変域が二つの文字の間であるのを示す内部コード */
#define UNUSDC  ((letter)-6)    /* マッチを失敗させるための内部コード */
#define REASIG  ((letter)-7)    /* 表の再設定を要求するための内部コード */
#define URBFCL  ((letter)-8)    /* 裏バッファの明示的クリアを要求する内部コード */
#define LTREOF  ((letter)-9)    /* romkan_next()、同getc()がEOFの代わりに返すもの
                                   （できればEOFと同じにしたいが…） */
#define REDRAW  ((letter)-10)   /* Wnnに変換行のredrawを要求する特殊コード */
#define NISEDL  ((letter)-11)   /* 偽デリートとして定数を使いたい人のために準備
                                   された定数（但し、使いたくなければ使わなくても
                                   いい）。変数nisedlにセットして使う */
#define SENDCH  ((letter)-12)   /* 変換バッファが空なら、次の文字を無条件に
                                   上に送るようWnnに要求するエスケープコード。 */


        /* romkan_init3()の引数のフラグに使う値 */

#define RK_CHMOUT 0x1           /* モードチェンジを知らせるコードを返すか？ */
#define RK_KEYACK 0x2           /* キーインに対し必ず何かを返すか */
#define RK_DSPNIL 0x4           /* モード表示文字列無指定の時に空文字列を返すか（デフ
                                   ォルトはNULL）。互換性保持のため */
#define RK_NONISE 0x8           /* 偽コードを出さないようにするか */
#define RK_REDRAW 0x10          /* Wnn用特殊フラグ（redraw用のフラグを出すかどうか） */
#define RK_SIMPLD 0x20          /* deleteの動作を単純にするか */
#define RK_VERBOS 0x40          /* 使用する表の一覧をリポートするか */


        /* コードの区別に使うマクロ */

#define HG1BIT  (0x80000000)    /* 最上位ビットだよ */
#define SHUBET(X) ((letter)(X) >> (int)24)
                              /* 内部表現で、上１バイトを種別表現に使ってる */
#define LWRMSK(X) ((X) & ~(0xff000000)) /* 上１バイトを取り除く */
#define LWRCUT(X) ((X) &= ~(0xff000000))        /* 上１バイトを取り除く */
#define XY2INT(X, Y) (((X) << (int)24) | (Y))   /* これが１回しかYを評価しないことに
                                                   依存して書いてある箇所がある  注意！ */

#define is_HON(X) (SHUBET(X) == 0)      /* 本物の文字か */
#define NAIBEX(X) (0 < SHUBET(X) && SHUBET(X) < 0x80)   /* 内部表現を示す値か */
#define isNISE(X) (SHUBET(X) == 0x80)   /* 偽物の文字か（最上位ビットが立つ） */
#define isSPCL(X) (SHUBET(X) == 0xff)   /* rk_spclval.hで定義される値かどうか */
 /* NISEDLなどを含めた偽の文字であるかどうかを判定するには、~is_HON(X) か、
    isNISE(X) || isSPCL(X) として判定しないといけない。 */

#define toNISE(X) ((X) | HG1BIT)
#define to_HON(X) ((X) & ~HG1BIT)

 /* 互換性のため用意してある別名 */
#define REALCD(X) is_HON(X)
#define NISECD(X) isNISE(X)
#define SPCLCD(X) isSPCL(X)


         /** 半角文字のコードのdefine */

#ifndef HNKAK1
#  ifdef IKIS
#    define HNKAK1 0xA8
#  else
#    define HNKAK1 0x8E
#  endif
#endif

        /** rk_bltinfn.c の補完のためのマクロ
            （引数を複数回評価するものも多いので注意）*/

#define HKKBGN  (HNKAK1 * 0x100 + 0xA1) /* 半角カナの句点 */
#define HKKEND  (HNKAK1 * 0x100 + 0xDF) /*     〃    半濁点 */
#define HIRBGN  (0xA4A1)        /* ぁ */
#define HIREND  (0xA4F3) /* ん */       /* ひらがな："ぁ"〜"ん" */
#define KATBGN  (0xA5A1)        /* ァ */
#define KATEND  (0xA5F6) /* ヶ */       /* カタカナ："ァ"〜"ン"〜"ヶ" */

#define _to_kata(l) ((l) + (KATBGN - HIRBGN)) /** カタカナへ（定義域制限）*/
#define _to_hira(l) ((l) - (KATBGN - HIRBGN)) /** ひらがなへ（定義域制限）*/
#define is_hira(l) (HIRBGN <= (l) && (l) <= HIREND) /** ひらがなか？ */
#define is_kata(l) (KATBGN <= (l) && (l) <= KATEND) /** カタカナか？ */
#define is_kata2(l) (_to_kata(HIRBGN) <= (l) && (l) <= _to_kata(HIREND))
                                /** 対応するひらがなのあるカタカナか？ */
#define to_kata(l) (is_hira(l) ? _to_kata(l) : (l)) /** カタカナへ */
#define to_hira(l) (is_kata2(l) ? _to_hira(l) : (l))
                        /** ひらがなへ。「ヴヵヶ」はカタカナのまま残る。*/
#define is_hankata(l) (HKKBGN <= (l) && (l) <= HKKEND)
                                /** 半角カナ（句点などを含む）か？ */



        /* その他のマクロ関数群（引数を複数回評価するものも多いので注意） */

#define numberof(array) (sizeof(array) / sizeof(*array))

 /* ポインタをletterの列の末尾へもっていく。letter *lp */
#define totail(lp) {while(*(lp) != EOLTTR) (lp)++;}

 /* 文字列へのポインタをその文字列の最後尾へ。totailのchar版。char *sp */
#define strtail(sp) {while(*(sp)) (sp)++;}

        /* 限定版romkan_init3 */
#define romkan_init4(pathname, delchr, nisedl, keyin, bytecount, flg) \
        romkan_init3(pathname, delchr, nisedl, EOLTTR, \
                     keyin, bytecount, bytecount, 0, flg)

#define romkan_init5(pathname, delchr, keyin, bytecount, flg) \
        romkan_init4(pathname, delchr, toNISE(delchr), keyin, bytecount, flg)


#endif /* of ifndef EOLTTR */
