/*
 * $Id: rk_bltinfn.c,v 1.2 2001/06/14 18:16:09 ura Exp $
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
                        rk_bltinfn.c
                                                87.12. 6  訂 補

        変換用の組み込み関数のうち複雑なものを定義してある。
        全角←→半角の変換が主体。
***********************************************************************/
/*      Version 3.1     88/06/14        H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#endif
#include "rext.h"

 /* 半角文字のコードのdefine */
#define HKCHOU  (SS2 * 0x100 + 0xB0)    /* ┛ */
#define HKDKTN  (SS2 * 0x100 + 0xDE)    /* �� */
#define HKHNDK  (SS2 * 0x100 + 0xDF)    /* �� */
#define HKMARU  (SS2 * 0x100 + 0xA1)    /* ─ */
#define HKHRKG  (SS2 * 0x100 + 0xA2)    /* │ */
#define HKTJKG  (SS2 * 0x100 + 0xA3)    /* ┌ */
#define HKTTEN  (SS2 * 0x100 + 0xA4)    /* ┐ */
#define HKNKPT  (SS2 * 0x100 + 0xA5)    /* ┘ */

 /* 全角文字のコードのdefine */
#define CHOUON  (0xA1BC)        /* ー */
#define DAKUTN  (0xA1AB)        /* ゛ */
#define HNDAKU  (0xA1AC)        /* ゜ */
#define MNMARU  (0xA1A3) /* 。 */       /* 名前は MaNMARU（まんまる）の略 */
#define HRKKAG  (0xA1D6)        /* 「 */
#define TJIKAG  (0xA1D7)        /* 」 */
#define TOUTEN  (0xA1A2)        /* 、 */
#define NKPOTU  (0xA1A6)        /* ・ */


 /** ASCII文字→全角 */
#ifdef OMRON_LIB
static
#endif
  letter
to_zenalpha (l)
     fast letter l;             /* V3.1 */
{
  fast letter retval;           /* V3.1 */

  static uns_chr *data = (uns_chr *) "　！”＃＄％＆’（）＊＋，−．／０１２３４５６７８９：；＜＝＞？\
＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［￥］＾＿｀ａｂｃｄｅ\
ｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝￣";

  if (' ' <= l && l <= '~')
    {
      l = (l - ' ') << 1;
      retval = data[l] << 8;
      retval += data[++l];
      return (retval);
/* } else return(l); *//* V3.1 */
    }
  return (l);
}

static char *hankdata[] = {
  "ｧ", "ｱ", "ｨ", "ｲ", "ｩ", "ｳ", "ｪ", "ｴ", "ｫ", "ｵ",
  "ｶ", "ｶﾞ", "ｷ", "ｷﾞ", "ｸ", "ｸﾞ", "ｹ", "ｹﾞ", "ｺ", "ｺﾞ",
  "ｻ", "ｻﾞ", "ｼ", "ｼﾞ", "ｽ", "ｽﾞ", "ｾ", "ｾﾞ", "ｿ", "ｿﾞ",
  "ﾀ", "ﾀﾞ", "ﾁ", "ﾁﾞ", "ｯ", "ﾂ", "ﾂﾞ", "ﾃ", "ﾃﾞ", "ﾄ", "ﾄﾞ",
  "ﾅ", "ﾆ", "ﾇ", "ﾈ", "ﾉ",
  "ﾊ", "ﾊﾞ", "ﾊﾟ", "ﾋ", "ﾋﾞ", "ﾋﾟ", "ﾌ", "ﾌﾞ", "ﾌﾟ",
  "ﾍ", "ﾍﾞ", "ﾍﾟ", "ﾎ", "ﾎﾞ", "ﾎﾟ",
  "ﾏ", "ﾐ", "ﾑ", "ﾒ", "ﾓ",
  "ｬ", "ﾔ", "ｭ", "ﾕ", "ｮ", "ﾖ",
  "ﾗ", "ﾘ", "ﾙ", "ﾚ", "ﾛ",
  "ヮ", "ﾜ", "ヰ", "ヱ", "ｦ", "ﾝ",
  "ｳﾞ", "ヵ", "ヶ"
};                              /* 全角が混じってるので注意 */
/*
static  char    *hankdata[] = {
        "├","┗","┬","┣","┤","┳","┴","┫","┼","┻",
        "╋","╋��","┠","┠��","┯","┯��","┨","┨��","┷","┷��",
        "┿","┿��","┝","┝��","┰","┰��","┥","┥��","┸","┸��",
        "╂","╂��","��","�賎�","┓","��","�即�","��","�竪�","��","�懲�",
        "��","��","��","��","��",
        "��","�沸�","�沸�","��","�萌�","�萌�","��","�岬�","�岬�",
         "��","�宥�","�宥�","��","�率�","�率�",
        "��","��","��","��","��",
        "━","��","┃","��","┏","��",
        "��","��","��","��","��",
        "ヮ","��","ヰ","ヱ","└","��",
        "┳��","ヵ","ヶ"
};*//* 全角が混じってるので注意 */

 /**    上のhankdataが、実際に使う半角コードを表していないとき、実際のものに
        修正する。初期設定時に一回だけ呼ぶ */
#ifdef OMRON_LIB
static
#endif
  void
hank_setup ()
{
  fast int i;                   /* V3.1 */
  fast char *s, orig_hnkak1;    /* V3.1 */

  orig_hnkak1 = *hankdata[0];
  /*     *hankdata[] での半角文字の１バイトめ。半角文字の１バイトめだけが異なる
     ような他機種に移植するときは、SS2のdefineを変えればOK。但しromkanの
     ソース中の半角文字（このファイルにのみ存在）もコンバートして、その機種
     に合わせるほうが望ましい。しかし、エディタでこのファイルを修正したり
     する場合に、半角文字の扱いがうまくいかない場合があるので、特に
     コンバートをしなくとも動作するように処置はしてある。それが、この
     hank_setup()である。hankdataは、初期設定時に hank_setup()によって
     実際の半角コードに直される。 */

  if (orig_hnkak1 == (char) SS2)
    return;
  for (i = 0; i < numberof (hankdata); i++)
    {
      for (s = hankdata[i]; *s; s += 2)
        if (*s == orig_hnkak1)
          *s = SS2;
    }
}

 /** かな→半角カタカナ。結果が二文字になることもある。*/
#ifdef OMRON_LIB
static
#endif
  void
to_hankata (in, outp)
     fast letter in, **outp;    /* V3.1 */
{
  fast uns_chr *p, c;           /* V3.1 */
  fast letter *out;             /* V3.1 */

  out = *outp;
  switch (in)
    {
    case CHOUON:
      *out++ = HKCHOU;
      break;
    case DAKUTN:
      *out++ = HKDKTN;
      break;
    case HNDAKU:
      *out++ = HKHNDK;
      break;
    case MNMARU:
      *out++ = HKMARU;
      break;
    case HRKKAG:
      *out++ = HKHRKG;
      break;
    case TJIKAG:
      *out++ = HKTJKG;
      break;
    case TOUTEN:
      *out++ = HKTTEN;
      break;
    case NKPOTU:
      *out++ = HKNKPT;
      break;
    default:
      if (is_kata (in))
        {
          for (p = (uns_chr *) hankdata[in - KATBGN]; c = *p; p++)
            *out++ = (c << 8) + *++p;
        }
      else if (is_hira (in))
        {
          for (p = (uns_chr *) hankdata[in - HIRBGN]; c = *p; p++)
            *out++ = (c << 8) + *++p;
        }
      else
        {
          *out++ = in;
        }
    }
  *out = EOLTTR;
  *outp = out;
}

 /**    半角カタカナ→ひらがな。但し、濁点を持つ文字を一つにまとめては
        くれないので注意。*/
#ifdef OMRON_LIB
static
#endif
  letter
to_zenhira (l)
     fast letter l;             /* V3.1 */
{
  fast letter retval;           /* V3.1 */

  static uns_chr *data = (uns_chr *) "。「」、・をぁぃぅぇぉゃゅょっーあいうえおかきくけこさしすせそた\
ちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわん゛゜";

  if (is_hankata (l))
    {
      l = (l - HKKBGN) << 1;
      retval = data[l] << 8;
      retval += data[++l];
      return (retval);
    }
  else
    return (l);
}

 /**    半角カタカナ→全角。但し、濁点を持つ文字を一つにまとめては
        くれないので注意。*/
#ifdef OMRON_LIB
static
#endif
  letter
to_zenkata (l)
     fast letter l;             /* V3.1 */
{
  return (is_hankata (l) ? (l = to_zenhira (l), to_kata (l)) : l);
}

 /* ビットベクタの構成 */
#define bitvec(b0, b1, b2, b3, b4, b5, b6, b7) (                         \
        (char)b0 | ((char)b1 << 1) | ((char)b2 << 2) | ((char)b3 << 3) | ((char)b4 << 4) | ((char)b5 << 5) | \
        ((char)b6 << 6) | ((char)b7 << 7)                                                \
)

 /** charの配列 h をビットベクタと見てその第iビットをチェックする */
#define bitlook(h, i) (h[(i) >> 3] & (1 << ((i) & 7)))

#define KATRPT  0xA1B3          /* ヽ */
#define HIRRPT  0xA1B5          /* ゝ */
#define KATA_U  0xA5A6          /* ウ */
#define KAT_VU  0xA5F4          /* ヴ */
#define HIR_KA  0xA4AB          /* か */
#define HIR_HO  0xA4DB          /* ほ */
#define KAT_KA  0xA5AB          /* カ */
#define KAT_HO  0xA5DB          /* ホ */
#define HIR_HA  0xA4CF          /* は */
#define KAT_HA  0xA5CF          /* ハ */

 /**    後ろに半濁点をくっつける。結果は一又は二文字。*/
#ifdef OMRON_LIB
static
#endif
  void
handakuadd (in, outp)
     fast letter in, **outp;    /* V3.1 */
{
  if ((HIR_HA <= in && in <= HIR_HO) ? 0 == (in - HIR_HA) % 3 : (KAT_HA <= in && in <= KAT_HO && 0 == (in - KAT_HA) % 3))
    {
      *(*outp)++ = in + 2;
    }
  else
    {
      *(*outp)++ = in;
      *(*outp)++ = HNDAKU;
    }
  **outp = EOLTTR;
}

 /**    後ろに濁点をくっつける。結果は一又は二文字。*/
#ifdef OMRON_LIB
static
#endif
  void
dakuadd (in, outp)
     fast letter in, **outp;    /* V3.1 */
{
  static char flgbit[] = {
    bitvec (1, 0, 1, 0, 1, 0, 1, 0),    /* かがきぎくぐけげ */
    bitvec (1, 0, 1, 0, 1, 0, 1, 0),    /* こごさざしじすず */
    bitvec (1, 0, 1, 0, 1, 0, 1, 0),    /* せぜそぞただちぢ */
    bitvec (0, 1, 0, 1, 0, 1, 0, 0),    /* っつづてでとどな */
    bitvec (0, 0, 0, 0, 1, 0, 0, 1),    /* にぬねのはばぱひ */
    bitvec (0, 0, 1, 0, 0, 1, 0, 0),    /* びぴふぶぷへべぺ */
    bitvec (1, 0, 0, 0, 0, 0, 0, 0)     /* ほ */
  };
  fast letter c;                /* V3.1 */

  if ((HIR_KA <= in && in <= HIR_HO) ? (c = in - HIR_KA, 1) : (KAT_KA <= in && in <= KAT_HO && (c = in - KAT_KA, 1)))
    {
      if (bitlook (flgbit, c))
        {
          *(*outp)++ = in + 1;
        }
      else
        {
          *(*outp)++ = in;
          *(*outp)++ = DAKUTN;
        }
    }
  else
    switch (in)
      {
      case KATRPT:
      case HIRRPT:
        *(*outp)++ = in + 1;
        break;
      case KATA_U:
        *(*outp)++ = KAT_VU;
        break;
      default:
        *(*outp)++ = in;
        *(*outp)++ = DAKUTN;
      }
  **outp = EOLTTR;
}

 /** inで与えられたコードをbase進の数字にしてoutpに入れる。*/
#ifdef OMRON_LIB
static
#endif
  void
to_digit (in, base, outp)
     fast letter in, base, **outp;      /* V3.1 */
{
  fast letter c;                /* V3.1 */
  extern letter vtol ();        /* V3.1 */

  if (c = in, c /= base)
    to_digit (c, base, outp);
  *(*outp)++ = vtol (in % base);
  **outp = EOLTTR;
}
