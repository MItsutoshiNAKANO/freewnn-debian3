/*
 *  $Id: cvt_key.c,v 1.5 2013/09/02 11:01:39 itisango Exp $
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
                        convert_key.c                    ＿＿＿
                                                87/12/19｜訂補｜
                                                         ￣￣￣
        タイムアウト処理を伴う、ファンクションキーコンバート処理。
***********************************************************************/
/*      Version 4.0
 */
#include <signal.h>
#include <setjmp.h>
#include "cvt_head.h"

/*
        keyin1()を呼び出すためには、次の手続きが必要です。

    (1) tgetent()（4.2BSD）もしくはsetupterm()（System V）によって、
        キーボードのterm情報が入っているエントリを読み込みます。
    (2) convert_getstrs() によって、読んだ情報を専用のエリアにコピーします。
        これによって、他のエントリのterm情報を読み込んでも、(1)で得た情報が
        失われなくなります。
    (3) convert_key_setup()を呼び、変換の初期設定をします。

        convert_getterm()によって、(1)と(2)が一度にできます。
        また、convert_key_init()によって(2)と(3)が、
        convert_getterm_init()によって(1)から(3)までが、それぞれ一度にできます。

        なお、これらの convert_ で始まるライブラリ関数は、convert_read.c にて
        定義されています。

        4.2BSDの場合に旧版で呼ぶ必要のあったgetterm_fkeydata()は、
        不必要になったため廃止されました。
*/

extern struct CONVCODE tbl[];
extern int cnv_tbl_cnt;         /* convert table count */


 /** intの配列 h をビットベクタとみなし、第iビットをチェックあるいは立てる */
#define BITONP(h, i) (h[i / BITSIZ] &  (1 << (i % BITSIZ)))
#define BITOFP(h, i) (!BITONP(h, i))
#define BIT_UP(h, i) (h[i / BITSIZ] |= (1 << (i % BITSIZ)))
#define BITDWN(h, i) (h[i / BITSIZ] &= ~(1 << (i % BITSIZ)))

 /** 変換コードのチェックとコード変換 */
int
key_check (inbuf, conv_tbl, tbl_cnt, check_flg)
     int inbuf[];               /* ソースストリング */
     struct CONVCODE conv_tbl[];        /* コード変換テーブル */
     int tbl_cnt;
     int check_flg[];
{
  int dist, base;
  char *code_p;
  int i;

  for (base = 0; inbuf[base] != -1; base++)
    {
      for (dist = 0; dist < tbl_cnt; dist++)
        {
          if (BITONP (check_flg, dist) && conv_tbl[dist].fromkey != 0)
            {
              code_p = conv_tbl[dist].fromkey + base;
              if (*code_p == (char) inbuf[base])
                {
                  if (*(code_p + 1) == (char) 0)
                    {
                      /* マッチした */
                      for (i = 0, base++; (inbuf[i] = inbuf[base]) != -1; i++, base++);
                      return (conv_tbl[dist].tokey);
                    }
                  /* まだマッチしていない */
                }
              else
                {
                  BITDWN (check_flg, dist);     /* 無効 */
                }
            }
        }
    }

  /* ビットベクタ check_flg[] の第0〜tblcntビットに立ったまま残っている
     ものがあるか調べる。 */
  for (i = 0; i < tbl_cnt / BITSIZ; i++)
    {
      if (check_flg[i])
        return (-1);
    }
  if ((tbl_cnt %= BITSIZ) && (check_flg[i] & ~(~0 << tbl_cnt)))
    return (-1);
  /* return -1 … まだ未決定の物がある */

  return (-2);                  /* 変換対象となる物はない */
}

 /** 指定された変換テーブルに従ってコード変換する。*/
int
convert_key (inkey, conv_tbl, tbl_cnt, matching_flg, in_buf)
     int (*inkey) ();           /* キー入力関数 */
     struct CONVCODE conv_tbl[];        /* 変換テーブル */
     int tbl_cnt;               /* conv_tbl[] の個数 */
     int matching_flg;          /* マッチングしなかったストリングの処理指定
                                   0 : 返値として返す
                                   1 : そのストリングは捨てる   */
     char *in_buf;
{
#define MAX     20              /* キー入力バッファの最大値 */

  static int inbuf[MAX];        /* キー入力バッファ */
  /* バッファの終端は、-1 で示される。 */

  int out_cnt;                  /* 出力バッファの出力カウント */

  static int buf_cnt = 0;       /* inbuf の入力時のカウンタ   */

  int check_flg[CHANGE_MAX];
  /* ビットベクタとして扱われ、マッチング時に対象となっているconv_tbl[]
     を示す。1の時対象となり、0で非対象 */

  int i, c, flg = 0;            /* work */

  for (i = 0; i < div_up (tbl_cnt, BITSIZ); check_flg[i++] = ~0);
  /* 配列check_flgをビットベクタ扱いし、その第0〜tbl_cnt ビットを立てる。
     但し、実際はその少し先まで立つ */

  for (;;)
    {
      if (flg != 0 || buf_cnt == 0)
        {
          inbuf[buf_cnt] = (*inkey) (); /* 一文字入力 */
          in_buf[buf_cnt] = (char) (inbuf[buf_cnt] & 0xff);
          if (inbuf[buf_cnt] == -1)
            {
              if (buf_cnt > 0)
                {
                  c = -2;       /* タイムアウト */
                  goto LABEL;
                }
              else
                {
                  continue;
                }
            }
          else
            {
              inbuf[++buf_cnt] = -1;    /* ターミネータ */
            }
        }
      flg++;

      if (buf_cnt >= MAX - 1)
        {
          in_buf[0] = '\0';
          return (-1);          /* ERROR */
        }

      c = key_check (inbuf, conv_tbl, tbl_cnt, check_flg);
    LABEL:
      switch (c)
        {
        case -1:                /* 未決定 */
          continue;

        case -2:                /* 変換対象でないことが決定した */
          buf_cnt--;
          out_cnt = 0;
          c = inbuf[out_cnt++];
          for (i = 0; inbuf[i] != -1; inbuf[i++] = inbuf[out_cnt++]);
          if (matching_flg != 0)
            {
              flg = 0;
              continue;
            }
          in_buf[0] = '\0';
          return (c);

        default:                /* 変換されたコード */
          in_buf[buf_cnt] = '\0';
          buf_cnt = 0;
          return (c);
        }
    }
}

 /** コード変換を伴うキー入力関数 */
int
keyin1 (get_ch, in_buf)
     int (*get_ch) (void);          /* getchar() と同様の関数 */
     char *in_buf;
{
  int ret;

  for (;;)
    {
      if (cnv_tbl_cnt == 0)
        {
          ret = (*get_ch) ();
          if (ret >= 0)
            return (ret);
        }
      else
        {
          return (convert_key (get_ch, tbl, cnv_tbl_cnt, 0, in_buf));
        }
    }
}
