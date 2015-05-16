/*
 *  $Id: cplib.h $
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

/**  cWnn  Version 1.1   **/


#ifndef min
#define min(a,b) ((int)(a) > (int)(b)? (b):(a))
#endif

#define YIN_LEN         10
#define PY_LEN          10      /*   'ChuangŽÀ', 'ZhuangŽÀ'  */
#define PY_LEN_W        7       /* for w_char  */

#define PY_MARK       'P'       /* for PinYincode: to know if need to change to */
#define ZY_MARK       'Z'       /* for PinYincode: to know if need to change to */
#define CWNN_PINYIN     0       /* For Pinyin */
#define CWNN_ZHUYIN     1       /* For Zhuyin */


#define  PY_EOF         0x8ec0  /* PY_EOF is end charactor of one PinYin
                                   must be a 2 bytes code.  HUANG */
#define  ZY_EOF_0       0x8ec0  /* ZhuYin end character(no sisheng): 'ŽÀ' */
#define  ZY_EOF_1       0x8ec1  /* ZhuYin end character(sisheng 1 ): 'ŽÁ' */
#define  ZY_EOF_2       0x8ec2  /* ZhuYin end character(sisheng 2 ): 'ŽÂ' */
#define  ZY_EOF_3       0x8ec3  /* ZhuYin end character(sisheng 3 ): 'ŽÃ' */
#define  ZY_EOF_4       0x8ec4  /* ZhuYin end character(sisheng 4 ): 'ŽÄ' */

#define  isZY_EOF(X)  ( ((int)(X) >= ZY_EOF_0 && (int)(X) <= ZY_EOF_4 )? 1 : 0 )

#define PY_NUM_SHENGMU  24      /* ShengMu table size of PinYin */
#define PY_NUM_YUNMU    39      /* YunMu table size of PinYin */
#define ZY_NUM_SHENGMU  24      /* ShengMu table size of ZhuYin */
#define ZY_NUM_YUNMU    41      /* YunMu table size of ZhuYin */

#define EMPTY_SHENG_RAW 0       /* position of ShengMu EMPTY in ShengMu
                                   table  */
#define EMPTY_YUN_RAW 0         /* position of YunMu EMPTY in YunMu
                                   table  */
#define X_SHENG_RAW     20      /* position of ShengMu X in ShengMu 
                                   table  */

/* YINcode creating is based on PinYin */
/* isyincod_d():  Check it is in the domain of Pinyin. To check if it is a 
   Pinyin, you need to use cp_isyincod() wihich checks the PinYin table  */
#define  _cwnn_isyincod_d(c)  ( ((c & 0x80) &&          \
                                (!(c & 0x8000)) &&              \
                                (c & 0x7f) >= 0x20 &&           \
                                (((int)c >> (int)8) & 0x7f) >= 0x20 )   \
                              ?1:0 )    /* if is in YINcode's limite */

#define _cwnn_sisheng(YINcod)   ( ((YINcod & 0x100) == 0x100)?  \
                           ((YINcod & 0x03 ) + 1): 0 )

#define _cwnn_yincod_0(YINcod)    ((YINcod) & 0xfefc)

/* Shengraw based on PinYin table */
#define Shengraw(YINcod) (((int)((YINcod - 0x20a0) & 0x7c)>>(int)2) + 0x01)

/* Yunraw based on PinYin table  */
#define Yunraw(YINcod)  ((int)(((YINcod) - 0x20a0) & 0x7e00) >> (int)9)


/* to see if the char is a start char of a pinyin without sisheng */
#define py0_first_ch(X) (((int)(X)>'A' && (int)(X)<='Z' &&  \
                          (X)!='E' && (X)!='O'&& \
                          (X)!='I' && (X)!='U' && (X)!='V') || \
                          (X)=='a'||(X)=='e'||(X)=='o' || (X)=='n'? 1: 0)

/* to see if the char is a start char of a zhuyin */

    /* for sisheng */
#define S_S_YOMI(X)     ( ((int)(X&0xff)>=0xa1)&&((int)(X&0xff)<=0xbf))||((int)(X>>8)==0x8e)
#define py_first_ch(X)  ( py0_first_ch(X) || S_S_YOMI(X) )
#define zy_first_ch(X)   ( ((int)(X&0xff) >= 0xc0 && (int)(X&0xff) <= 0xe9 && \
                            ((int)(X>>(int)8)) == 0x8e) )

#define _cwnn_has_sisheng(YINcod)       ( (( (YINcod)& 0x0100) != 0 ) ?1:0)

extern unsigned char last_mark;

extern char *py_shengmu_tbl[];  /* PinYin ShengMu table */
extern char *py_yunmu_tbl[];    /* PinYin YunMu table   */
extern char *zy_shengmu_tbl[];  /* ZhuYin ShengMu table */
extern char *zy_yunmu_tbl[];    /* ZhuYin YunMu table   */

extern int pinyin_tbl[];        /* PinYin table         */
extern int zhuyin_tbl[];        /* ZhuYin table         */
