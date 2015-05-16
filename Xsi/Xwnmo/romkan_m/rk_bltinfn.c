/*
 * $Id: rk_bltinfn.c,v 1.3.2.1 1999/02/08 05:55:01 yamasita Exp $
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
                                                87.12. 6  Äû Êä

        ÊÑ´¹ÍÑ¤ÎÁÈ¤ß¹þ¤ß´Ø¿ô¤Î¤¦¤ÁÊ£»¨¤Ê¤â¤Î¤òÄêµÁ¤·¤Æ¤¢¤ë¡£
        Á´³Ñ¢«¢ªÈ¾³Ñ¤ÎÊÑ´¹¤¬¼çÂÎ¡£
***********************************************************************/
/*      Version 3.1     88/06/14        H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#endif
#include "rext.h"

 /* È¾³ÑÊ¸»ú¤Î¥³¡¼¥É¤Îdefine */
#define HKCHOU  (SS2 * 0x100 + 0xB0)    /* ¨° */
#define HKDKTN  (SS2 * 0x100 + 0xDE)    /* ¨Þ */
#define HKHNDK  (SS2 * 0x100 + 0xDF)    /* ¨ß */
#define HKMARU  (SS2 * 0x100 + 0xA1)    /* ¨¡ */
#define HKHRKG  (SS2 * 0x100 + 0xA2)    /* ¨¢ */
#define HKTJKG  (SS2 * 0x100 + 0xA3)    /* ¨£ */
#define HKTTEN  (SS2 * 0x100 + 0xA4)    /* ¨¤ */
#define HKNKPT  (SS2 * 0x100 + 0xA5)    /* ¨¥ */

 /* Á´³ÑÊ¸»ú¤Î¥³¡¼¥É¤Îdefine */
#define CHOUON  (0xA1BC)        /* ¡¼ */
#define DAKUTN  (0xA1AB)        /* ¡« */
#define HNDAKU  (0xA1AC)        /* ¡¬ */
#define MNMARU  (0xA1A3) /* ¡£ */       /* Ì¾Á°¤Ï MaNMARU¡Ê¤Þ¤ó¤Þ¤ë¡Ë¤ÎÎ¬ */
#define HRKKAG  (0xA1D6)        /* ¡Ö */
#define TJIKAG  (0xA1D7)        /* ¡× */
#define TOUTEN  (0xA1A2)        /* ¡¢ */
#define NKPOTU  (0xA1A6)        /* ¡¦ */


 /** ASCIIÊ¸»ú¢ªÁ´³Ñ */
#ifdef OMRON_LIB
static
#endif
  letter
to_zenalpha (l)
     fast letter l;             /* V3.1 */
{
  fast letter retval;           /* V3.1 */

  static uns_chr *data = (uns_chr *) "¡¡¡ª¡É¡ô¡ð¡ó¡õ¡Ç¡Ê¡Ë¡ö¡Ü¡¤¡Ý¡¥¡¿£°£±£²£³£´£µ£¶£·£¸£¹¡§¡¨¡ã¡á¡ä¡©\
¡÷£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ð£Ñ£Ò£Ó£Ô£Õ£Ö£×£Ø£Ù£Ú¡Î¡ï¡Ï¡°¡²¡®£á£â£ã£ä£å\
£æ£ç£è£é£ê£ë£ì£í£î£ï£ð£ñ£ò£ó£ô£õ£ö£÷£ø£ù£ú¡Ð¡Ã¡Ñ¡±";

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
  "Ž§", "Ž±", "Ž¨", "Ž²", "Ž©", "Ž³", "Žª", "Ž´", "Ž«", "Žµ",
  "Ž¶", "Ž¶ŽÞ", "Ž·", "Ž·ŽÞ", "Ž¸", "Ž¸ŽÞ", "Ž¹", "Ž¹ŽÞ", "Žº", "ŽºŽÞ",
  "Ž»", "Ž»ŽÞ", "Ž¼", "Ž¼ŽÞ", "Ž½", "Ž½ŽÞ", "Ž¾", "Ž¾ŽÞ", "Ž¿", "Ž¿ŽÞ",
  "ŽÀ", "ŽÀŽÞ", "ŽÁ", "ŽÁŽÞ", "Ž¯", "ŽÂ", "ŽÂŽÞ", "ŽÃ", "ŽÃŽÞ", "ŽÄ", "ŽÄŽÞ",
  "ŽÅ", "ŽÆ", "ŽÇ", "ŽÈ", "ŽÉ",
  "ŽÊ", "ŽÊŽÞ", "ŽÊŽß", "ŽË", "ŽËŽÞ", "ŽËŽß", "ŽÌ", "ŽÌŽÞ", "ŽÌŽß",
  "ŽÍ", "ŽÍŽÞ", "ŽÍŽß", "ŽÎ", "ŽÎŽÞ", "ŽÎŽß",
  "ŽÏ", "ŽÐ", "ŽÑ", "ŽÒ", "ŽÓ",
  "Ž¬", "ŽÔ", "Ž­", "ŽÕ", "Ž®", "ŽÖ",
  "Ž×", "ŽØ", "ŽÙ", "ŽÚ", "ŽÛ",
  "¥î", "ŽÜ", "¥ð", "¥ñ", "Ž¦", "ŽÝ",
  "Ž³ŽÞ", "¥õ", "¥ö"
};                              /* Á´³Ñ¤¬º®¤¸¤Ã¤Æ¤ë¤Î¤ÇÃí°Õ */
/*
static  char    *hankdata[] = {
        "¨§","¨±","¨¨","¨²","¨©","¨³","¨ª","¨´","¨«","¨µ",
        "¨¶","¨¶¨Þ","¨·","¨·¨Þ","¨¸","¨¸¨Þ","¨¹","¨¹¨Þ","¨º","¨º¨Þ",
        "¨»","¨»¨Þ","¨¼","¨¼¨Þ","¨½","¨½¨Þ","¨¾","¨¾¨Þ","¨¿","¨¿¨Þ",
        "¨À","¨À¨Þ","¨Á","¨Á¨Þ","¨¯","¨Â","¨Â¨Þ","¨Ã","¨Ã¨Þ","¨Ä","¨Ä¨Þ",
        "¨Å","¨Æ","¨Ç","¨È","¨É",
        "¨Ê","¨Ê¨Þ","¨Ê¨ß","¨Ë","¨Ë¨Þ","¨Ë¨ß","¨Ì","¨Ì¨Þ","¨Ì¨ß",
         "¨Í","¨Í¨Þ","¨Í¨ß","¨Î","¨Î¨Þ","¨Î¨ß",
        "¨Ï","¨Ð","¨Ñ","¨Ò","¨Ó",
        "¨¬","¨Ô","¨­","¨Õ","¨®","¨Ö",
        "¨×","¨Ø","¨Ù","¨Ú","¨Û",
        "¥î","¨Ü","¥ð","¥ñ","¨¦","¨Ý",
        "¨³¨Þ","¥õ","¥ö"
};*//* Á´³Ñ¤¬º®¤¸¤Ã¤Æ¤ë¤Î¤ÇÃí°Õ */

 /**    ¾å¤Îhankdata¤¬¡¢¼ÂºÝ¤Ë»È¤¦È¾³Ñ¥³¡¼¥É¤òÉ½¤·¤Æ¤¤¤Ê¤¤¤È¤­¡¢¼ÂºÝ¤Î¤â¤Î¤Ë
        ½¤Àµ¤¹¤ë¡£½é´üÀßÄê»þ¤Ë°ì²ó¤À¤±¸Æ¤Ö */
#ifdef OMRON_LIB
static
#endif
  void
hank_setup ()
{
  fast int i;                   /* V3.1 */
  fast char *s, orig_hnkak1;    /* V3.1 */

  orig_hnkak1 = *hankdata[0];
  /*     *hankdata[] ¤Ç¤ÎÈ¾³ÑÊ¸»ú¤Î£±¥Ð¥¤¥È¤á¡£È¾³ÑÊ¸»ú¤Î£±¥Ð¥¤¥È¤á¤À¤±¤¬°Û¤Ê¤ë
     ¤è¤¦¤ÊÂ¾µ¡¼ï¤Ë°Ü¿¢¤¹¤ë¤È¤­¤Ï¡¢SS2¤Îdefine¤òÊÑ¤¨¤ì¤ÐOK¡£Ã¢¤·romkan¤Î
     ¥½¡¼¥¹Ãæ¤ÎÈ¾³ÑÊ¸»ú¡Ê¤³¤Î¥Õ¥¡¥¤¥ë¤Ë¤Î¤ßÂ¸ºß¡Ë¤â¥³¥ó¥Ð¡¼¥È¤·¤Æ¡¢¤½¤Îµ¡¼ï
     ¤Ë¹ç¤ï¤»¤ë¤Û¤¦¤¬Ë¾¤Þ¤·¤¤¡£¤·¤«¤·¡¢¥¨¥Ç¥£¥¿¤Ç¤³¤Î¥Õ¥¡¥¤¥ë¤ò½¤Àµ¤·¤¿¤ê
     ¤¹¤ë¾ì¹ç¤Ë¡¢È¾³ÑÊ¸»ú¤Î°·¤¤¤¬¤¦¤Þ¤¯¤¤¤«¤Ê¤¤¾ì¹ç¤¬¤¢¤ë¤Î¤Ç¡¢ÆÃ¤Ë
     ¥³¥ó¥Ð¡¼¥È¤ò¤·¤Ê¤¯¤È¤âÆ°ºî¤¹¤ë¤è¤¦¤Ë½èÃÖ¤Ï¤·¤Æ¤¢¤ë¡£¤½¤ì¤¬¡¢¤³¤Î
     hank_setup()¤Ç¤¢¤ë¡£hankdata¤Ï¡¢½é´üÀßÄê»þ¤Ë hank_setup()¤Ë¤è¤Ã¤Æ
     ¼ÂºÝ¤ÎÈ¾³Ñ¥³¡¼¥É¤ËÄ¾¤µ¤ì¤ë¡£ */

  if (orig_hnkak1 == (char) SS2)
    return;
  for (i = 0; i < numberof (hankdata); i++)
    {
      for (s = hankdata[i]; *s; s += 2)
        if (*s == orig_hnkak1)
          *s = SS2;
    }
}

 /** ¤«¤Ê¢ªÈ¾³Ñ¥«¥¿¥«¥Ê¡£·ë²Ì¤¬ÆóÊ¸»ú¤Ë¤Ê¤ë¤³¤È¤â¤¢¤ë¡£*/
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

 /**    È¾³Ñ¥«¥¿¥«¥Ê¢ª¤Ò¤é¤¬¤Ê¡£Ã¢¤·¡¢ÂùÅÀ¤ò»ý¤ÄÊ¸»ú¤ò°ì¤Ä¤Ë¤Þ¤È¤á¤Æ¤Ï
        ¤¯¤ì¤Ê¤¤¤Î¤ÇÃí°Õ¡£*/
#ifdef OMRON_LIB
static
#endif
  letter
to_zenhira (l)
     fast letter l;             /* V3.1 */
{
  fast letter retval;           /* V3.1 */

  static uns_chr *data = (uns_chr *) "¡£¡Ö¡×¡¢¡¦¤ò¤¡¤£¤¥¤§¤©¤ã¤å¤ç¤Ã¡¼¤¢¤¤¤¦¤¨¤ª¤«¤­¤¯¤±¤³¤µ¤·¤¹¤»¤½¤¿\
¤Á¤Ä¤Æ¤È¤Ê¤Ë¤Ì¤Í¤Î¤Ï¤Ò¤Õ¤Ø¤Û¤Þ¤ß¤à¤á¤â¤ä¤æ¤è¤é¤ê¤ë¤ì¤í¤ï¤ó¡«¡¬";

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

 /**    È¾³Ñ¥«¥¿¥«¥Ê¢ªÁ´³Ñ¡£Ã¢¤·¡¢ÂùÅÀ¤ò»ý¤ÄÊ¸»ú¤ò°ì¤Ä¤Ë¤Þ¤È¤á¤Æ¤Ï
        ¤¯¤ì¤Ê¤¤¤Î¤ÇÃí°Õ¡£*/
#ifdef OMRON_LIB
static
#endif
  letter
to_zenkata (l)
     fast letter l;             /* V3.1 */
{
  return (is_hankata (l) ? (l = to_zenhira (l), to_kata (l)) : l);
}

 /* ¥Ó¥Ã¥È¥Ù¥¯¥¿¤Î¹½À® */
#define bitvec(b0, b1, b2, b3, b4, b5, b6, b7) (                         \
        (char)b0 | ((char)b1 << 1) | ((char)b2 << 2) | ((char)b3 << 3) | ((char)b4 << 4) | ((char)b5 << 5) | \
        ((char)b6 << 6) | ((char)b7 << 7)                                                \
)

 /** char¤ÎÇÛÎó h ¤ò¥Ó¥Ã¥È¥Ù¥¯¥¿¤È¸«¤Æ¤½¤ÎÂèi¥Ó¥Ã¥È¤ò¥Á¥§¥Ã¥¯¤¹¤ë */
#define bitlook(h, i) (h[(i) >> 3] & (1 << ((i) & 7)))

#define KATRPT  0xA1B3          /* ¡³ */
#define HIRRPT  0xA1B5          /* ¡µ */
#define KATA_U  0xA5A6          /* ¥¦ */
#define KAT_VU  0xA5F4          /* ¥ô */
#define HIR_KA  0xA4AB          /* ¤« */
#define HIR_HO  0xA4DB          /* ¤Û */
#define KAT_KA  0xA5AB          /* ¥« */
#define KAT_HO  0xA5DB          /* ¥Û */
#define HIR_HA  0xA4CF          /* ¤Ï */
#define KAT_HA  0xA5CF          /* ¥Ï */

 /**    ¸å¤í¤ËÈ¾ÂùÅÀ¤ò¤¯¤Ã¤Ä¤±¤ë¡£·ë²Ì¤Ï°ìËô¤ÏÆóÊ¸»ú¡£*/
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

 /**    ¸å¤í¤ËÂùÅÀ¤ò¤¯¤Ã¤Ä¤±¤ë¡£·ë²Ì¤Ï°ìËô¤ÏÆóÊ¸»ú¡£*/
#ifdef OMRON_LIB
static
#endif
  void
dakuadd (in, outp)
     fast letter in, **outp;    /* V3.1 */
{
  static char flgbit[] = {
    bitvec (1, 0, 1, 0, 1, 0, 1, 0),    /* ¤«¤¬¤­¤®¤¯¤°¤±¤² */
    bitvec (1, 0, 1, 0, 1, 0, 1, 0),    /* ¤³¤´¤µ¤¶¤·¤¸¤¹¤º */
    bitvec (1, 0, 1, 0, 1, 0, 1, 0),    /* ¤»¤¼¤½¤¾¤¿¤À¤Á¤Â */
    bitvec (0, 1, 0, 1, 0, 1, 0, 0),    /* ¤Ã¤Ä¤Å¤Æ¤Ç¤È¤É¤Ê */
    bitvec (0, 0, 0, 0, 1, 0, 0, 1),    /* ¤Ë¤Ì¤Í¤Î¤Ï¤Ð¤Ñ¤Ò */
    bitvec (0, 0, 1, 0, 0, 1, 0, 0),    /* ¤Ó¤Ô¤Õ¤Ö¤×¤Ø¤Ù¤Ú */
    bitvec (1, 0, 0, 0, 0, 0, 0, 0)     /* ¤Û */
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

 /** in¤ÇÍ¿¤¨¤é¤ì¤¿¥³¡¼¥É¤òbase¿Ê¤Î¿ô»ú¤Ë¤·¤Æoutp¤ËÆþ¤ì¤ë¡£*/
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
