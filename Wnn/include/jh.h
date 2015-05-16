/*
 *  $Id: jh.h,v 1.5 2005/04/10 15:26:37 aonoto Exp $
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

#ifndef WNN_JH_H
#define WNN_JH_H

/* 
 * header file for jisho structure program.
 */

/* The following defin and structure definition
 * are concerned with ASCII (UJIS) files.
 */

#define COMMENT "\\comment"
#define TOTAL "\\total"
#define ASC_GIJI "\\giji"
#define HINSI "\\hinsi"
#ifdef CHINESE
#define CHINSI "\\cixing"
#define PINYIN "\\Pinyin"
#define ZHUYIN "\\Zhuyin"
#define BIXING "\\BiXing"
#endif
#define DIC_NO "\\dic_no"

#define REV_NORMAL 2
#define REVERSE 1
#define NORMAL  0

#ifndef JS
struct je
{
  w_char *yomi;
  w_char *kan;                  /* Historically kanji is used so use kan. */
  w_char *comm;
  UCHAR *kanji;
  unsigned int hinsi;
#ifdef  CONVERT_with_SiSheng
  unsigned int ss;
#endif
  unsigned int hindo;
  int serial;                   /* Only used for rev_dic */
};

extern struct je **jeary;
#endif /* JS */

extern w_char file_comment[];
extern w_char hinsi_list[];

/*
 * Used in atod and others parameters.
 */

#define HEAP_PER_LINE 10        /* avelage of kanji + comment bytes */
#define YOMI_PER_LINE 3         /* avelage of yomi length(in w_char) */
#define LINE_SIZE 1024
#define BADLMAX 3
#define YOMI_KINDS (1 << 16)    /* Yomi characters Maximal */
#define MAX_ENTRIES 70000       /* default of max-entries for atod */
#define DEF_ENTRIES 10000

#define HEAPINC 1000


#endif  /* WNN_JH_H */
